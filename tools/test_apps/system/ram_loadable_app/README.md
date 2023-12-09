| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# RAM loadable app Example

Starts a FreeRTOS task to print "Hello World". The segments of this application are
fully linked and run in internal RAM.

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## How to use example

### Hardware Required

This example should be able to run on any commonly available ESP32 development board.

### Configure the project

```
idf.py menuconfig
```

This step is optional, the default settings in `sdkconfig.defaults` are already set to enable the ram_loadable app feature.

`CONFIG_APP_BUILD_TYPE_RAM` is enable  by default so that all programs and data are linked into internal RAM. For more information about `CONFIG_APP_BUILD_TYPE_RAM` you can refer to the description in menuconfig.

(Enabling `APP_BUILD_TYPE_PURE_RAM_APP` option IDF will not compile the `spi_flash` related code into bin, which will save a lot of internal ram space. For `esp32` target, limited by its RAM layout, the available RAM space for the app is too small to accommodate this example without this option enabled, so this option is selected by default for esp32 target.)

### Build and Load to RAM

Build the project and load it to the chip's internal RAM, then run monitor tool to view serial output:

```
idf.py set-target {target name}

idf.py build

esptool.py -p PORT --no-stub load_ram build/ram_loadable_app.bin

idf.py -p PORT monitor --no-reset
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

(For ram_loadable_app, after the chip is reset, it will start from flash by default, so the program will be executed directly after loading to ram. This is the reason why we use `--no-reset`. Besides, manually opening idf.py monitor will lose part of the log at startup because the serial port cannot be opened in time, so it is recommended to use a separate serial converter to monitor the output of the UART TX pin)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.
