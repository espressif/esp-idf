#
# This is a project Makefile for the test subproject.
#

PROJECT_NAME := unit_test_test

# Include the components directory of the main application:
#
EXTRA_COMPONENT_DIRS := $(realpath ../components)

# Set the components to include the tests for.
# This can be overriden from the command line
# (e.g. 'make TEST_COMPONENTS=xxxx flash monitor')
#
TEST_COMPONENTS ?= testable

include $(IDF_PATH)/make/project.mk
