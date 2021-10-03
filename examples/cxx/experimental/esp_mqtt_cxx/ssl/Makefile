#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#
PROJECT_NAME := mqtt_tcp_cxx

EXTRA_COMPONENT_DIRS := $(IDF_PATH)/examples/common_components/protocol_examples_common
EXTRA_COMPONENT_DIRS += $(IDF_PATH)/examples/cxx/experimental/esp_mqtt_cxx/components
EXTRA_COMPONENT_DIRS += $(IDF_PATH)/examples/cxx/experimental/experimental_cpp_component

CXXFLAGS += -std=gnu++17
include $(IDF_PATH)/make/project.mk
