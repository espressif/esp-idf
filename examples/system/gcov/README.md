# Blink Example with Coverage Info

See the README.md file in the upper level 'examples' directory for more information about examples.

GCC has useful feature which allows to generate code coverage information. Generated data show how many times every program execution paths has been taken.
Basing on coverage data developers can detect untested pieces of code and also it gives valuable information about critical (frequently used) execution paths.
In general case when coverage option is enabled GCC generates additional code to accumulate necessary data and save them into files. File system is not always available
in ESP32 based projects or size of the file storage can be very limited to keep all the coverage data. To overcome those limitations IDF provides functionality
to transfer the data to the host and save them on host file system. The data transfer is done via JTAG.
This example shows how to generate coverage information for the program.


## How To Gather Coverage Info

Below are the steps which should be performed to obtain coverage info. Steps 1-3 are already done for this example project. They should be performed if user wants to fork new IDF-based project and needs to collect coverage info.

1. Enable application tracing module in menuconfig. Choose `Trace memory` in `Component config -> Application Level Tracing -> Data Destination`.
2. Enable coverage info generation for necessary source files. To do this add the following line to the 'component.mk' files of your project:
`CFLAGS += --coverage`
It will enable coverage info for all source files of your component. If you need to enable the option only for certain files you need to add the following line for every file of interest:
`gcov_example.o: CFLAGS += --coverage`
Replace `gcov_example.o` with path to your file.
3. Add call to `esp_gcov_dump` function in your program. This function will wait for command from the host and dump coverage data. The exact place where to put call to `esp_gcov_dump` depends on the program.
Usually it should be placed at the end of the program execution (at exit). See `gcov_example.c` for example.
4. Build, flash and run program.
5. Wait until `esp_gcov_dump` is called. To detect this a call to `printf` can be used (see `gcov_example.c`) or, for example, you can use a LED to indicate the readiness to dump data.
6. Connect OpenOCD to the target and start telnet session with it.
7. Run the following OpenOCD command:
`esp32 gcov`
Example of the command output:

```
> esp32 gcov
Total trace memory: 16384 bytes
Connect targets...
Target halted. PRO_CPU: PC=0x400D0CDC (active)    APP_CPU: PC=0x00000000
esp32: target state: halted
Resume targets
Targets connected.
Open file '/home/alexey/projects/esp/esp-idf/examples/system/gcov/build/main/gcov_example.gcda'
Open file '/home/alexey/projects/esp/esp-idf/examples/system/gcov/build/main/gcov_example.gcda'
Open file '/home/alexey/projects/esp/esp-idf/examples/system/gcov/build/main/gcov_example_func.gcda'
Open file '/home/alexey/projects/esp/esp-idf/examples/system/gcov/build/main/gcov_example_func.gcda'
Disconnect targets...
Target halted. PRO_CPU: PC=0x400D17CA (active)    APP_CPU: PC=0x400D0CDC
esp32: target state: halted
Resume targets
Targets disconnected.
>
```

As shown in the output above there can be errors reported. This is because GCOV code tries to open non-existing coverage data files for reading before writing to them. It is normal situation and actually is not an error.
GCOV will save coverage data for every source file in directories for corresponding object files, usually under root build directory `build`.

## How To Process Coverage Info

There are several ways to process collected data. Two of the most common are:

1. Using `gcov` tool supplied along with xtensa toolchain. See [GCOV documentation](https://gcc.gnu.org/onlinedocs/gcc/Gcov.html) for details.
2. Using `lcov` and `genhtml` tools. This way allows to generate pretty looking coverage reports in html. This example shows how to add build target to generate such kind of reports.
Add the following lines to you project's `Makefile` after the line including `project.mk`:

```
GCOV := $(call dequote,$(CONFIG_TOOLPREFIX))gcov
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

Those lines add two build targets:
* `lcov-report` - generates html coverage report in `$(BUILD_DIR_BASE)/coverage_report/html` directory.
* `cov-data-clean` - removes all coverage data files and reports.

To generate report type the following command:
`make lcov-report`

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