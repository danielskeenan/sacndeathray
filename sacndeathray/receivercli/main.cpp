/**
 * @file main.cpp
 *
 * @author Dan Keenan
 * @date 11/29/2025
 * @copyright GPL-3.0-or-later
 */

#include "ProgramOptions.h"
#include "sacndeathray/common/EtcPalLogHandler.h"
#include "sacndeathray/common/NetInt.h"
#include "sacndeathray/common/SpdlogQtMessageHandler.h"
#include "sacndeathray/config.h"
#include "sacndeathray/receiverlib/ReceiverRunner.h"
#include <iostream>
#include <sacn/cpp/common.h>
#include <spdlog/spdlog.h>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QHostAddress>
#include <QLibraryInfo>
#include <QLocale>
#include <QTimer>
#include <QTranslator>

using namespace sacndeathray;

struct CommandLineParseResult
{
    enum class Status {
        Ok,
        Error,
        VersionRequested,
        HelpRequested,
        ListNetIntsRequested,
    };
    Status statusCode = Status::Ok;
    std::optional<QString> errorText;
    ProgramOptions programOptions;

    CommandLineParseResult(const Status status, const QString &message = {}) :
        statusCode(status), errorText(message)
    {}
    CommandLineParseResult(const ProgramOptions &options) : programOptions(options) {}
};

CommandLineParseResult parseCommandLine(QCommandLineParser &parser)
{
    using Status = CommandLineParseResult::Status;

    ProgramOptions programOptions;

    parser.setApplicationDescription(qApp->translate("main", "Stress-test sACN networks"));
    auto helpOpt = parser.addHelpOption();
    auto versionOpt = parser.addVersionOption();
    // Log level
    QCommandLineOption logLevelOpt(QStringList{"l", "log"});
    logLevelOpt.setDescription(qApp->translate("main", "Log Level"));
    logLevelOpt.setValueName(qApp->translate("main", "0-5"));
    logLevelOpt.setDefaultValue(QString::number(spdlog::level::critical - spdlog::level::info));
    logLevelOpt.setFlags(QCommandLineOption::ShortOptionStyle);
    parser.addOption(logLevelOpt);
    // Port
    QCommandLineOption portOpt(QStringList{"p", "port"});
    portOpt.setDescription(qApp->translate("main", "Port to communicate with the receiver on"));
    portOpt.setValueName(qApp->translate("main", "port"));
    portOpt.setDefaultValue(QString::number(config::kMessagePort));
    parser.addOption(portOpt);
    // Network interface
    QCommandLineOption netIntOpt(QStringList{"i", "interface"});
    netIntOpt.setDescription(qApp->translate("main", "Network interface name"));
    netIntOpt.setValueName(qApp->translate("main", "name"));
    netIntOpt.setDefaultValue({});
    parser.addOption(netIntOpt);
    // List network interfaces
    QCommandLineOption listNetIntOpt(QStringList{"list-interfaces"});
    listNetIntOpt.setDescription(
        qApp->translate("main", "List all network interfaces on this system and exit"));
    parser.addOption(listNetIntOpt);

    // Parse
    if (!parser.parse(qApp->arguments())) {
        return CommandLineParseResult(Status::Error, parser.errorText());
    }
    if (parser.isSet(helpOpt)) {
        return CommandLineParseResult(Status::HelpRequested);
    }
    if (parser.isSet(versionOpt)) {
        return CommandLineParseResult(Status::VersionRequested);
    }
    if (parser.isSet(listNetIntOpt)) {
        return CommandLineParseResult(Status::ListNetIntsRequested);
    }

    // Log level
    if (parser.isSet(logLevelOpt)) {
        bool ok = false;
        const auto logLevel = parser.value(logLevelOpt).toInt(&ok);
        if (!ok || logLevel < 0 || logLevel > 5) {
            return CommandLineParseResult(
                Status::Error,
                qApp->translate(
                    "main", "Log level must be a number between 0 (critical) and 5 (debug)."));
        }
        // spdlog's level values are "backwards" (i.e. higher number means less logging) so we need to reverse it.
        programOptions.logLevel = static_cast<spdlog::level::level_enum>(
            spdlog::level::critical - logLevel);
    }

    // Port
    if (parser.isSet(portOpt)) {
        bool ok;
        const auto port = parser.value(portOpt).toUInt(&ok);
        if (!ok || port < 1025 || port > 65535) {
            return CommandLineParseResult(
                Status::Error,
                qApp->translate("main", "Port must be between 1025 and 65535 inclusive."));
        }
        programOptions.port = port;
    } else {
        programOptions.port = config::kMessagePort;
    }

    // Network interface (required)
    if (parser.isSet(netIntOpt)) {
        auto netInt = QNetworkInterface::interfaceFromName(parser.value(netIntOpt));
        if (!netInt.isValid()) {
            return CommandLineParseResult(
                Status::Error,
                qApp->translate(
                        "main",
                        "Network interface  %1 does not exist. Use --%2 to list all interfaces.")
                    .arg(parser.value(netIntOpt))
                    .arg(listNetIntOpt.names().back()));
        }
        programOptions.netInt = netInt;
    } else {
        return CommandLineParseResult(
            Status::Error,
            qApp->translate("main", "--%1 is required").arg(netIntOpt.names().back()));
    }

    return programOptions;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setOrganizationName(config::kProjectOrganizationName);
    app.setOrganizationDomain(config::kProjectOrganizationDomain);
    app.setApplicationName(config::kProjectName);
    app.setApplicationVersion(config::kProjectVersion);

    // Logging
    // Qt
    qInstallMessageHandler(&spdlogQtMessageHandler);

    // Command-line options.
    QCommandLineParser parser;
    const auto parseResult = parseCommandLine(parser);
    switch (parseResult.statusCode) {
    case CommandLineParseResult::Status::Ok:
        break;
    case CommandLineParseResult::Status::Error:
        std::cerr << qPrintable(parser.errorText()) << std::endl << std::endl;
        parser.showHelp(1);
    case CommandLineParseResult::Status::VersionRequested:
        parser.showVersion();
        Q_UNREACHABLE_RETURN(0);
    case CommandLineParseResult::Status::HelpRequested:
        parser.showHelp();
        Q_UNREACHABLE_RETURN(0);
    case CommandLineParseResult::Status::ListNetIntsRequested:
        printNetInts(std::cout);
        return(0);
    }

    spdlog::set_level(parseResult.programOptions.logLevel);

    SPDLOG_INFO(
        "STARTING {app} (receiver) v{version} build {build}",
        fmt::arg("app", config::kProjectName),
        fmt::arg("version", config::kProjectVersion),
        fmt::arg("build", config::kProjectCommitSha));
    SPDLOG_INFO("Qt v{}", QT_VERSION_STR);

    SPDLOG_INFO("Initializing");
    etcpal_init(ETCPAL_FEATURES_ALL);
    etcpal::Logger etcPalLogger;
    etcPalLogger.SetSyslogAppName(config::kProjectName);
    EtcPalLogHandler etcPalLogHandler;
    if (!etcPalLogger.Startup(etcPalLogHandler)) {
        SPDLOG_ERROR(
            "Error starting the logger for the sACN subsystem.  Some logs will not be available.");
    }

    // Init sACN
    auto result = sacn::Init(etcPalLogger);
    if (!result.IsOk()) {
        SPDLOG_CRITICAL("Error initializing the sACN subsystem: {}", result.ToString());
    }

    // Translations
    SPDLOG_DEBUG("Installing translations");
    QTranslator qtTranslator;
    if (qtTranslator.load(
            QLocale::system(), "qtbase", "_", QLibraryInfo::path(QLibraryInfo::TranslationsPath))) {
        SPDLOG_DEBUG("Found qtbase translations");
        app.installTranslator(&qtTranslator);
    }
    QTranslator appTranslator;
    if (appTranslator
            .load(QLocale::system(), QString(":/i18n/%1_en_US.qm").arg(app.applicationName()))) {
        SPDLOG_DEBUG("Found app translations");
        app.installTranslator(&appTranslator);
    }

    // Run program from the event loop.
    ReceiverRunner runner(parseResult.programOptions, qApp);
    QObject::connect(&runner, &ReceiverRunner::finished, qApp, &QCoreApplication::quit);
    QTimer::singleShot(0, [&runner]() { runner.start(); });
    const auto ret = app.exec();

    // Cleanup
    sacn::Deinit();
    etcPalLogger.Shutdown();
    etcpal_deinit(ETCPAL_FEATURES_ALL);

    return ret;
}
