#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

PROJECT_NAME := gcov_example

include $(IDF_PATH)/make/project.mk

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
