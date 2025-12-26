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
*   **Configuration Driven**: Manage signal codes and target commands via a simple `config.properties` file.

## Tech Stack

*   **Language**: C++26
*   **Build System**: CMake 4.1+.
*   **Dependencies**:
  *   `libudev`: For monitoring system hardware events.
  *   `librtlsdr`: For interfacing with RTL-SDR dongles.
  *   `libusb-1.0`: For USB communication.
  *   `rtl_433` (External tool): Must be installed and available in the system PATH.

## Getting Started

### Prerequisites

* Ensure you have the following installed on your Linux system:
  * `libudev-dev`
  * `rtl-433`
* RTL-SDR compatible USB dongle (e.g., RTL2832U-based device)

### Usage

Run the executable with a shell command argument to execute when the configured RF signal is detected:
> rtl-signald '\<command>'
 
Alternatively, you can manually change the `config.properties` file to configure the desired signal code and command:
> command=echo "Remote Code Execution!"\
> signal_code=123456789