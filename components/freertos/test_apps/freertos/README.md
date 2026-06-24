| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-P4 | ESP32-S2 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | -------- | -------- | --------- |



## Manual Code Coverage Workflow

This test_app can be set up to enable FreeRTOS code coverage using GCOV via JTAG and AppTrace.

Follow these steps to manually build, flash, run, and collect code coverage from a FreeRTOS app:

### 1. **Configure the project and build the application**
The sdkconfig.ci.code_coverage enables code coverage settings and other FreeRTOS options to maximize coverage.
```sh
idf.py set-target <target>      # e.g., esp32s3
idf.py -DSDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.ci.code_coverage" build flash monitor
```

### 2. **Flash the Device**
```sh
idf.py -p <PORT> flash
```

### 3. **Launch OpenOCD**
Start OpenOCD in a new terminal (adjust for your board/target if needed):
```sh
openocd -f board/esp32<target>-builtin.cfg
```

### 4. **Run Tests**
Interact with the device over serial (`idf.py monitor -p <PORT>`) and run tests.

### 5. **Collect Coverage Data**
Flush coverage data regularly, and collect `.gcda` files from the device using a telnet session
```sh
telnet localhost 4444
```
Then run
```sh
esp gcov
```

All `.gcda` files will be retrieved into your build directory.
Make sure to regularly collect `.gcda` files to avoid memory overflow problems.
Coverage data should be collected before tests that reset the target.
The coverage data is cumulative.

### 6. **Generate Coverage Report**
Adjust the `gcov` tool to be used based on the target
```sh
xtensa-esp32s3-elf-gcov -b \
  ../esp-idf/freertos/CMakeFiles/__idf_freertos.dir/FreeRTOS-Kernel/*.gcda

xtensa-esp32s3-elf-gcov -b \
  ../esp-idf/freertos/CMakeFiles/__idf_freertos.dir/esp_additions/*gcda
```
Then run:
```sh
mkdir coverage_report
cd coverage_report
mkdir html
▶ gcovr \
  --gcov-use-existing-files \
  --gcov-keep \
  --root $IDF_PATH/components/freertos \
  --html-details \
  --output html/index.html \
  .
```
This generates a detailed, navigable HTML coverage report in `html` folder.
