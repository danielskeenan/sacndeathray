/**
 * @file main.cpp
 *
 * @author Dan Keenan
 * @date 11/23/2025
 * @copyright GPL-3.0-or-later
 */

#include "ProgramOptions.h"
#include "Runner.h"
#include "sacndeathray/common/EtcPalLogHandler.h"
#include "sacndeathray/common/SpdlogQtMessageHandler.h"
#include "sacndeathray/config.h"
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
    logLevelOpt.setDefaultValue(QString::number(spdlog::level::critical - programOptions.logLevel));
    logLevelOpt.setFlags(QCommandLineOption::ShortOptionStyle);
    parser.addOption(logLevelOpt);
    // Rate
    QCommandLineOption rateOpt(QStringList{"rate"});
    rateOpt.setDescription(qApp->translate("main", "Transmit rate (Hz)"));
    rateOpt.setValueName(qApp->translate("main", "1-44"));
    rateOpt.setDefaultValue(QString::number(programOptions.rate));
    parser.addOption(rateOpt);
    // Universes
    QCommandLineOption universesOpt(QStringList{"u", "universe"});
    universesOpt.setDescription(qApp->translate(
        "main", "Universes to use; specify more than once to use more than one universe"));
    universesOpt.setValueName(qApp->translate("main", "univ"));
    universesOpt.setDefaultValues([&programOptions]() {
        QStringList r;
        for (const auto &universe : programOptions.universes) {
            r.push_back(QString::number(universe));
        }
        return r;
    }());
    parser.addOption(universesOpt);
    // Receiver address
    parser.addPositionalArgument("receiver", qApp->translate("main", "Receiver IP Address"));
    // Port
    QCommandLineOption portOpt(QStringList{"p", "port"});
    portOpt.setDescription(qApp->translate("main", "Port to communicate with the receiver on"));
    portOpt.setValueName(qApp->translate("main", "port"));
    portOpt.setDefaultValue(QString::number(programOptions.port));
    parser.addOption(portOpt);

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

    // Rate
    if (parser.isSet(rateOpt)) {
        bool ok = false;
        const auto rate = parser.value(rateOpt).toDouble(&ok);
        if (!ok || rate < 1 || rate > 44) {
            return CommandLineParseResult(
                Status::Error,
                qApp->translate("main", "Transmit rate must be a number between 1 and 44 (Hz)."));
        }
        programOptions.rate = rate;
    }

    // Universes
    if (parser.isSet(universesOpt)) {
        programOptions.universes.clear();
        for (const auto &universeStr : parser.values(universesOpt)) {
            bool ok;
            const auto universe = universeStr.toUInt(&ok);
            if (!ok || universe < kSacnMinimumUniverse || universe > kSacnMaximumUniverse) {
                return CommandLineParseResult(
                    Status::Error,
                    qApp->translate("main", "Universe must be between %1 and %2 inclusive.")
                        .arg(kSacnMinimumUniverse)
                        .arg(kSacnMaximumUniverse));
            }
            programOptions.universes.push_back(universe);
        }
    }

    // Receiver address
    if (parser.positionalArguments().size() != 1) {
        return CommandLineParseResult(
            Status::Error, qApp->translate("main", "Receiver address is required."));
    }
    if (!programOptions.receiverAddress.setAddress(parser.positionalArguments().front())) {
        return CommandLineParseResult(
            Status::Error, qApp->translate("main", "Receiver address is malformed."));
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
    }

    spdlog::set_level(parseResult.programOptions.logLevel);

    SPDLOG_INFO(
        "STARTING {app} v{version} build {build}",
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
    Runner runner(parseResult.programOptions, qApp);
    QObject::connect(&runner, &Runner::finished, qApp, &QCoreApplication::quit);
    QTimer::singleShot(0, &runner, &Runner::start);
    const auto ret = app.exec();

    // Cleanup
    sacn::Deinit();
    etcPalLogger.Shutdown();
    etcpal_deinit(ETCPAL_FEATURES_ALL);

    return ret;
}
