# sACN Death Ray

This program transmits a variable number of universes from a transmitter machine and checks for missed packets on a
receiver machine.

## Usage

There are two separate programs: a transmitter and a receiver.

1. Start the receiver program, noting the machine's IP Address on the desired network interface.
2. Start the transmitter program, passing the desired network interface, universes, and receiver's IP Address.
3. Wait for the test complete.
4. Observe the results.

### Transmitter

`sacndeathray_transmitter.exe [options] receiver`

`-?`, `-h`, `--help`
: Displays help on commandline options.

`-v`, `--version`
: Displays version information.

`-l <0-5>`, `--log <0-5>`
: Log level. Higher number means more verbose logging.

`--rate <1-44>`
: Transmit rate (Hz). Defaults to 22 Hz.

`-u <univ>`, `--universe <univ>`
: Universes to use. Enter a single number or a range (e.g. "1-10"). Specify more than once to use more than one
universe or universe range. Defaults to Univ 1 only.

`-p <port>`, `--port <port>`
: Port to communicate with the receiver on. Defaults to 62406.

`-i <name>`, `--interface <name>` (Required)
: Network interface name. Use `--list-interfaces` to find the correct name to use.

`--list-interfaces`
: List all network interfaces on this system and exit. Use this option to determine which interface name to pass to
`--interface`.

`-d <seconds>`, `--duration <seconds>`
: Duration to test for. Defaults to 60 seconds.

`receiver` (Required)
: Receiver IP Address.

### Receiver

`sacndeathray_receiver.exe [options]`

`-?`, `-h`, `--help`
: Displays help on commandline options.

`-v`, `--version`
: Displays version information.

`-l <0-5>`, `--log <0-5>`
: Log level. Higher number means more verbose logging.

`-p <port>`, `--port <port>`
: Port to communicate with the receiver on. Defaults to 62406.

`-i <name>`, `--interface <name>` (Required)
: Network interface name. Use `--list-interfaces` to find the correct name to use.

`--list-interfaces`
: List all network interfaces on this system and exit. Use this option to determine which interface name to pass to
`--interface`.
