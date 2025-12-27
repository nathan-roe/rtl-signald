# RTL Signal & Device Monitor

A C++23 utility designed to bridge the gap between Radio Frequency (RF) signals and system automation. This application monitors for specific RF signals via SDR (Software Defined Radio) and tracks USB device connectivity to trigger user-defined system commands.

## Purpose

The primary goal of this project is to provide a lightweight daemon or service that listens for 433MHz signals (using `rtl_433`) and monitors hardware changes via `udev`. When a recognized signal is received or a specific hardware event occurs, the application executes a pre-configured shell command.

This is particularly useful for:
*   Integrating RF remotes/sensors with Linux desktop automation.
*   Running specific scripts automatically when a USB device is plugged in or removed.
*   Creating a DIY "smart home" trigger system using inexpensive SDR hardware.

## Key Features

*   **RF Signal Detection**: Interfaces with `rtl_433` to capture and decode sub-GHz wireless signals.
*   **Hardware Monitoring**: Uses `libudev` to detect real-time USB "add" and "remove" events.
*   **Interactive Training**: If no signal code is configured, the app enters a "learning mode" to help you identify and save the correct RF signal code from your device.
*   **Debounced Execution**: Includes built-in signal debouncing to prevent a single button press from triggering multiple command executions.
*   **Configuration Driven**: Manage signal codes and target commands via a simple `rtlsignald.properties` file.

## Tech Stack

*   **Language**: C++23
*   **Build System**: CMake
*   **Dependencies**:
    *   `libudev`: For monitoring system hardware events.
    *   `librtlsdr`: For interfacing with RTL-SDR dongles.
    *   `libusb-1.0`: For USB communication.
    *   `rtl_433` (External tool): Must be installed and available in the system PATH.

## Getting Started

### Prerequisites

Before running the setup scripts, you must define several environment variables that dictate how the service and configuration are generated.

| Variable | Description | Example |
| :--- | :--- | :--- |
| `RTL_SIGNALD_CODE` | The RF signal code to listen for. | `12345678` |
| `RTL_SIGNALD_PATH` | Full path to the `rtl-signald` binary. | `/usr/local/bin/rtl-signald` |
| `RTL_SIGNALD_COMMAND` | The command to execute on signal detection. | `systemctl suspend` |
| `RTL_USER` | The system user to run the service and own the config. | `pi` |
| `RTL_GROUP` | The system group for the service. | `pi` |

#### Automated Setup

1.  Export the required environment variables (see table above).
2.  Run the `setup.sh` script. This will:
    *   Install `libudev-dev` and `rtl-433`.
    *   Create the configuration file at `~/.config/rtlsignald.properties` with your specified signal code.
    *   Set proper ownership for the configuration file.

#### Manual Setup

1.  Ensure you have the following installed:
    *   `libudev-dev`
    *   `rtl-433`
2.  RTL-SDR compatible USB dongle (e.g., RTL2832U-based device).
3.  Create the directory `mkdir -p ~/.config`.
4.  Create `~/.config/rtlsignald.properties` with the following content:
    ```properties
    signal_code=YOUR_CODE_HERE
    ```

### Usage

Run the executable directly to test your configuration:
> ./rtl-signald --command 'echo "Signal Received"'

If no `signal_code` is found in the properties file, the application will enter "Learning Mode" to help you identify the code for your remote.

### Running as a Daemon

To run `rtl-signald` as a persistent background service (Systemd), use the provided `service.sh` script.

1.  **Environment Variables**: Ensure `RTL_SIGNALD_PATH`, `RTL_SIGNALD_COMMAND`, `RTL_USER`, and `RTL_GROUP` are exported in your current shell.
2.  **Installation**: Run `./service.sh`.
    *   This script generates a Systemd unit file at `/etc/systemd/system/rtlsignald.service`.
    *   It reloads the daemon, enables the service, and starts it.
    *   The script will automatically tail the logs (`journalctl`) so you can verify the service started correctly.

The service is configured to:
*   Start after the network is available.
*   Restart automatically on failure.
*   Run with the `--require-code` flag to ensure the configuration file has a defined signal code. 

To manage the service manually:
```bash
sudo systemctl start rtlsignald
sudo systemctl stop rtlsignald
sudo systemctl status rtlsignald
```