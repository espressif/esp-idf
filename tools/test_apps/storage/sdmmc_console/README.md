| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# SDMMC Test Application

This app is used to test the SDMMC protocol layer (in `sdmmc` component), as well as SDMMC and SDSPI drivers (in `driver` component).

The app serves two purposes:

1. It allows various Unity test cases for SDMMC protocol layer features: probing, data transfer, etc.
2. It also allows running lower level operations from the console, which makes it useful as a debugging/hacking/experimentation tool when investigating SDMMC issues.
   - Initializing the SDMMC host in various modes
   - Probing the card
   - (more to come: sending commands, reading/writing data, etc.)

## SDMMC test app code overview

The app consists of several components:
- `main` — small amount of code to initialize the console and register all the commands.
- `components/cmd_unity` — console command to run Unity tests.
- `components/cmd_sdmmc` — console commands for SDMMC low-level operations.
- `components/sdmmc_test_board` — contains pin mappings for various development boards, and APIs for tests to access them.
- `components/sdmmc_test` — contains the actual test cases.

## Supported boards

* ESP32-WROVER-KIT
* ESP32 eMMC test board (black board with two SD card slots and eMMC)
* ESP32-S3 USB_OTG v1. (Also works with an older ESP32-S2 variant of the board.)
* ESP32-S3 eMMC test board (white board with two SD card slots and eMMC)
* ESP32-S3-EYE
* Breakout board for ESP32-C3 DevKitM-1
* Custom board definition: specify the pin mapping in `menuconfig`.

## Using the app

### Select the target, configure, build and flash the app

1. Choose the chip target, and open menuconfig:
    ```bash
    idf.py set-target esp32
    idf.py menuconfig
    ```
2. Select the development board in `SDMMC Test Board Configuration` menu. This will select the correct pins for the SD card slot on the board.
3. Save the configuration and exit menuconfig.
4. Build the app:
    ```bash
    idf.py build
    ```
5. Flash and monitor:
    ```bash
    idf.py flash monitor
    ```

### Advanced: multiple build configurations side-by-side

It is often useful to verify changes in `sdmmc` component on multiple chips or development boards. To do this, it is possible to build the app multiple times, with different configurations, keeping the builds in separate directories.

1. Build the app for the first target. This command does multiple things: selects the target, sets the build directory to `build_esp32`, and puts the sdkconfig file into the build directory:
    ```bash
    idf.py -D IDF_TARGET=esp32 -B build_esp32 -D SDKCONFIG=build_esp32/sdkconfig build
    ```
2. Flash and monitor. Note that the build directory has to be specified with `-B` option:
    ```bash
    idf.py -B build_esp32 flash monitor
    ```
3. Now you can build the app for the second target in another build directory:
    ```bash
    idf.py -D IDF_TARGET=esp32s3 -B build_esp32s3 -D SDKCONFIG=build_esp32s3/sdkconfig build
    ```
4. Flash and monitor, again specifying the build directory:
    ```bash
    idf.py -B build_esp32s3 flash monitor
    ```

Compared to the `idf.py set-target` approach, this method allows keeping multiple build configurations side-by-side, and switching between them easily. If you have multiple terminal windows open, you can use one window per board. Set ESPPORT environment variable to the correct serial port in each window, and flash and monitor the app in each window with the corresponding build directory (`-B`) argument.

### Console commands

The app supports the following console commands:

- Common system commands: `help`, `restart`, `version`, `free`, `heap`
- Log control: `log_level <tag> <level>` — dynamically change the log level for a given tag. For example, `log_level sdmmc_req debug` will enable debug logging in sdmmc_transaction.c
- Running unit tests: `test [index|name|tag|*]`.
  - If no argument is given, prints the list of available tests.
  - If a test index is given, runs the test with that index.
  - If a test name is given, runs the test with that name.
  - If a test tag is given, runs all tests with that tag. You can negate the tag by prefixing it with `!`, for example `test ![sdspi]` will run all tests except those tagged with `[sdspi]`.
- SDMMC low-level commands: `sdmmc_host_init`, `sdmmc_host_deinit`, `card_init`, `card_info`. Refer to the `help` output for more details.

As most other IDF console applications, the app supports line editing, commands history and tab completion.

### Running test cases

When running the tests, keep in mind that once an SD card has been initialized in SPI mode, it cannot be re-initalized in SD mode without first powering it off. This means that on boards without an SD card power control circuit, it is not possible to run all the tests in one go with `test *` command. Run SDMMC tests first, then SDSPI tests: `test [sdmmc]` and then `test [sdspi]`. If you need to run SDMMC test again, power cycle the card first.

On chips without an SDMMC host controller only SDSPI tests are compiled. In this case, `test *` can be used to run all tests.

### Skipped tests

To make the app compatible with various development boards without having lots of ifdefs in test cases, the tests will be ignored if the board is not compatible with the test case. For example, on boards with just one SD card slot (Slot 1), every test which uses Slot 0 is skipped.

For example, when running sdspi tests on ESP32-WROVER-KIT you will see something like this:
```
12 Tests 0 Failures 5 Ignored
```

This means that the 5 tests which use Slot 0 were skipped.
