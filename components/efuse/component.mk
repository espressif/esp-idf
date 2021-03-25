#
# Component Makefile
# currently the only SoC supported; to be moved into Kconfig
TARGET := $(IDF_TARGET)

COMPONENT_SRCDIRS := $(TARGET) src
ifdef CONFIG_IDF_TARGET_ESP32
COMPONENT_OBJEXCLUDE := src/esp_efuse_api_key_esp32xx.o
else
COMPONENT_OBJEXCLUDE := src/esp_efuse_api_key_esp32.o
endif

COMPONENT_PRIV_INCLUDEDIRS := private_include $(TARGET)/private_include
COMPONENT_ADD_INCLUDEDIRS := include $(TARGET)/include
