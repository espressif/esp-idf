#
# Component Makefile
#

ifdef CONFIG_IDF_TARGET_ESP32
    COMPONENT_SRCDIRS := . eth_phy
else
    COMPONENT_CONFIG_ONLY := 1
endif
