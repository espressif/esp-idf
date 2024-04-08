| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Startup Time Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates the configuration settings to obtain the minimum possible startup time for an ESP-IDF application (i.e. time from initial reset until the `app_main()` function is running).

Note that some of the configuration settings in `sdkconfig.defaults` have trade-offs that may not be suitable for your project, see the "Applying To Your Own Project" section at the bottom for more details.

## How to use example

### Hardware Required

This example should be able to run on any commonly available ESP32 development board.

### Configure the project

This step is optional, the default settings in `sdkconfig.defaults` are already set to minimize startup time.

```
idf.py menuconfig
```

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

The example should have log output similar to the following:

```
W (34) spi_flash: Detected size(4096k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
I (37) example: App started!
```

All early log output is disabled in order to save time (it's also possible to disable the ROM log output immediately after reset, see below.)

Note that boot time reported in the log timestamp may vary depending on the chip target, chip revision, and eFuse configuration.

## Applying To Your Own Project

The file `sdkconfig.defaults` contains a range of setting which can be applied to any project in order for it to boot more rapidly.

However, note that some of these settings may have tradeoffs - for example not all hardware may support all flash modes and speeds, some applications would prefer the reliability of checking the application on every boot rather than waiting for a crash and then checking the application, and some applications will use features such as Secure Boot which require additional overhead on boot.

The `sdkconfig.defaults` file in this directory contains comments above each of the settings to optimize boot speed. To add the settings
to your own project, either search for the setting name in `menuconfig` or exit `menuconfig` and then copy-paste the setting lines at the end of your project's `sdkconfig` file.

## Additional Startup Speed

Removing the default boot log output printed by the ROM can shave several milliseconds off the SoC boot time. The default configuration doesn't make this change, as it is done via eFuse the change is permanent.

If you wish to make this change run `idf.py menuconfig`, navigate to "Boot ROM Behavior" and set the "Permanently change Boot ROM output" option.
