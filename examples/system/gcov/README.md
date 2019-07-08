# Blink Example with Coverage Info

See the README.md file in the upper level 'examples' directory for more information about examples.

GCC has useful feature which allows to generate code coverage information. Generated data show how many times every program execution paths has been taken.
Basing on coverage data developers can detect untested pieces of code and also it gives valuable information about critical (frequently used) execution paths.
In general case when coverage option is enabled GCC generates additional code to accumulate necessary data and save them into files. File system is not always available in ESP32 based projects or size of the file storage can be very limited to keep all the coverage data. To overcome those limitations IDF provides functionality to transfer the data to host and save them on its file system. Data transfer is done via JTAG.
This example shows how to generate coverage information for the program.

## How To Gather Coverage Info

There are two ways to collect gcov data:
* Hard-coded call to `esp_gcov_dump`.
* Instant run-time dumping w/o changes in your code via IDF's gcov debug stub.

### Generic Steps

Below are generic steps which should be performed to obtain coverage info. The steps are already done for this example project.

1. Enable application tracing module in menuconfig. Choose `Trace memory` in `Component config -> Application Level Tracing -> Data Destination`.
2. Enable GCOV to host interface in menuconfig `Component config -> Application Level Tracing -> GCOV to Host Enable`.
3. Enable coverage info generation for necessary source files. To do this add the following line to the 'component.mk' files of your project:
`CFLAGS += --coverage`
It will enable coverage info for all source files of your component. If you need to enable the option only for certain files the following line should be added for every file of interest:
`gcov_example.o: CFLAGS += --coverage`
Replace `gcov_example.o` with path to your file.

  For CMake-based build system, use `target_compile_options(${COMPONENT_LIB} PRIVATE --coverage)` or: `  set_source_files_properties(gcov_example.c PROPERTIES COMPILE_FLAGS --coverage`


### Hard-coded Dump Call

This method requires `esp_gcov_dump` to be called from your application's code. Below are additional steps which should be performed after the generic ones to obtain coverage info via hard-coded call. Step 1 is already done for this example project.

1. Add call to `esp_gcov_dump` function in your program. This function will wait for command from host and dump coverage data. The exact place where to put the call  depends on the program.
Usually it should be placed at the end of the program execution (at exit). But if you need to generate GCOV data incrementally `esp_gcov_dump` can be called multiple times. See `gcov_example.c` for example.
2. Build, flash and run program.
3. Wait until `esp_gcov_dump` is called. To detect this a call to `printf` can be used (see `gcov_example.c`) or, for example, you can use a LED to indicate the readiness to dump data.
Another way to detect call to `esp_gcov_dump` is to set breakpoint on that function, start target execution and wait for the target to be stopped. See the next section for respective GDB example.
4. Connect OpenOCD to the target and start telnet session with it.
5. Run the following OpenOCD command: `esp32 gcov dump`

Example of the command output:
```
> esp32 gcov dump
Total trace memory: 16384 bytes
Connect targets...
Target halted. PRO_CPU: PC=0x40088BC3 (active)    APP_CPU: PC=0x400D14E6 
Targets connected.
Open file 0x1 '/home/alexey/projects/esp/esp-idf/examples/system/gcov/build/main/gcov_example.gcda'
Open file 0x1 '/home/alexey/projects/esp/esp-idf/examples/system/gcov/build/main/gcov_example.gcda'
Open file 0x2 '/home/alexey/projects/esp/esp-idf/examples/system/gcov/build/main/gcov_example_func.gcda'
Open file 0x2 '/home/alexey/projects/esp/esp-idf/examples/system/gcov/build/main/gcov_example_func.gcda'
Disconnect targets...
Target halted. PRO_CPU: PC=0x400D14E6 (active)    APP_CPU: PC=0x400D14E6 
Targets disconnected.
```

#### Dump Using GDB

As it was said above breakpoint can be used to detect when `esp_gcov_dump` is called.
The following GDB commands can be used to dump data upon call to `esp_gcov_dump` automatically (you can put them into `gdbinit` file):
```
b esp_gcov_dump
commands
mon esp32 gcov dump
end
```
Note that all OpenOCD commands should be invoked in gdb as: `mon <oocd_command>`.

### Instant Run-Time Dump

Instant dump does not require to call `esp_gcov_dump`, so your application's code does not need to be modified. This method stops target at its current state and executes builtin IDF gcov debug stub function.
Having data dumped target resumes its execution. Below are the steps which should be performed to do instant dump. Step 1 is already done for this example project.

1. Enable OpenOCD debug stubs in menuconfig `Component config -> ESP32-specific -> OpenOCD debug stubs`.
2. Build, flash and run program.
3. Connect OpenOCD to the target and start telnet session with it.
4. Run the following OpenOCD command: `esp32 gcov`

Example of the command output:
```
> esp32 gcov
Total trace memory: 16384 bytes
Connect targets...
Target halted. PRO_CPU: PC=0x400D14DA (active)    APP_CPU: PC=0x400D14DA 
Targets connected.
Open file 0x1 '/home/alexey/projects/esp/esp-idf/examples/system/gcov/build/main/gcov_example.gcda'
Open file 0x1 '/home/alexey/projects/esp/esp-idf/examples/system/gcov/build/main/gcov_example.gcda'
Open file 0x2 '/home/alexey/projects/esp/esp-idf/examples/system/gcov/build/main/gcov_example_func.gcda'
Open file 0x2 '/home/alexey/projects/esp/esp-idf/examples/system/gcov/build/main/gcov_example_func.gcda'
Target halted. PRO_CPU: PC=0x400844CE (active)    APP_CPU: PC=0x400855E3 
Disconnect targets...
Targets disconnected.
>
```

### Coverage Data Accumulation

Coverage data from several dumps are automatically accumulated. So the resulting gcov data files contain statistics since the board reset. Every data dump updates files accordingly.
New data collection is started if target has been reset.

## How To Process Coverage Info

There are several ways to process collected data. Two of the most common are:

1. Using `gcov` tool supplied along with xtensa toolchain. See [GCOV documentation](https://gcc.gnu.org/onlinedocs/gcc/Gcov.html) for details.
2. Using `lcov` and `genhtml` tools. This way allows to generate pretty looking coverage reports in html. This example shows how to add build target to generate such kind of reports.
Add the following lines to you project's `Makefile` after the line including `project.mk`:

```
GCOV := $(call dequote,$(CONFIG_SDK_TOOLPREFIX))gcov
REPORT_DIR := $(BUILD_DIR_BASE)/coverage_report

lcov-report:
	echo "Generating coverage report in: $(REPORT_DIR)"
	echo "Using gcov: $(GCOV)"
	mkdir -p $(REPORT_DIR)/html
	lcov --gcov-tool $(GCOV) -c -d $(BUILD_DIR_BASE) -o $(REPORT_DIR)/$(PROJECT_NAME).info
	genhtml -o $(REPORT_DIR)/html $(REPORT_DIR)/$(PROJECT_NAME).info

cov-data-clean:
	echo "Remove coverage data files..."
	find $(BUILD_DIR_BASE) -name "*.gcda" -exec rm {} +
	rm -rf $(REPORT_DIR)

.PHONY: lcov-report cov-data-clean
```

For CMake-based build system, add the following lines to you project's `CMakeLists.txt` after the line including `project.cmake`:

```
include($ENV{IDF_PATH}/tools/cmake/gcov.cmake)
idf_create_coverage_report(${CMAKE_CURRENT_BINARY_DIR}/coverage_report)
idf_clean_coverage_report(${CMAKE_CURRENT_BINARY_DIR}/coverage_report)
```

Those lines add two build targets:
* `lcov-report` - generates html coverage report in `$(BUILD_DIR_BASE)/coverage_report/html` directory.
* `cov-data-clean` - removes all coverage data files and reports.

To generate report type the following command:
`make lcov-report`

  For CMake-based build system, type `cmake --build build/ --target lcov-report`

The sample output of the command is below:

```
Generating coverage report in: /home/alexey/projects/esp/esp-idf/examples/system/gcov/build/coverage_report
Using gcov: ~/projects/esp/crosstool-NG/builds/xtensa-esp32-elf/bin/xtensa-esp32-elf-gcov
Capturing coverage data from /home/alexey/projects/esp/esp-idf/examples/system/gcov/build
Found gcov version: 5.2.0
Scanning /home/alexey/projects/esp/esp-idf/examples/system/gcov/build for .gcda files ...
Found 2 data files in /home/alexey/projects/esp/esp-idf/examples/system/gcov/build
Processing main/gcov_example_func.gcda
Processing main/gcov_example.gcda
Finished .info-file creation
Reading data file /home/alexey/projects/esp/esp-idf/examples/system/gcov/build/coverage_report/gcov_example.info
Found 2 entries.
Found common filename prefix "/home/alexey/projects/esp/esp-idf/examples/system/gcov"
Writing .css and .png files.
Generating output.
Processing file main/gcov_example.c
Processing file main/gcov_example_func.c
Writing directory view page.
Overall coverage rate:
  lines......: 90.0% (18 of 20 lines)
  functions..: 100.0% (3 of 3 functions)
```

NOTE: Since `lcov` tool is not part of GCC bundle it can happen that format of GCOV binary data has been changed and your local version of `lcov` fails to understand it.
So it always better to use the latest `lcov` version from [LCOV repo](https://github.com/linux-test-project/lcov).
