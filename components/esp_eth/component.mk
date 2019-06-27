#
# Component Makefile
#
COMPONENT_ADD_INCLUDEDIRS := include
COMPONENT_SRCDIRS := src
COMPONENT_ADD_LDFRAGMENTS += linker.lf

ifndef CONFIG_IDF_TARGET_ESP32
    COMPONENT_OBJEXCLUDE += src/esp_eth_mac_esp32.o
endif
