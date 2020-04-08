#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := gcov_example

include $(IDF_PATH)/make/project.mk

GCOV := $(call dequote,$(CONFIG_SDK_TOOLPREFIX))gcov
REPORT_DIR := $(BUILD_DIR_BASE)/coverage_report

pre-cov-report:
	echo "Generating coverage report in: $(REPORT_DIR)"
	echo "Using gcov: $(GCOV)"
	mkdir -p $(REPORT_DIR)/html

lcov-report: | pre-cov-report
	echo "WARNING: lcov-report is deprecated. Please use gcovr-report instead."
	lcov --gcov-tool $(GCOV) -c -d $(BUILD_DIR_BASE) -o $(REPORT_DIR)/$(PROJECT_NAME).info
	genhtml -o $(REPORT_DIR)/html $(REPORT_DIR)/$(PROJECT_NAME).info

gcovr-report: | check_python_dependencies pre-cov-report
	cd $(BUILD_DIR_BASE)
	gcovr -r $(PROJECT_PATH) --gcov-executable $(GCOV) -s --html-details $(REPORT_DIR)/html/index.html

cov-data-clean:
	echo "Remove coverage data files..."
	find $(BUILD_DIR_BASE) -name "*.gcda" -exec rm {} +
	rm -rf $(REPORT_DIR)

.PHONY: lcov-report gcovr-report cov-data-clean
