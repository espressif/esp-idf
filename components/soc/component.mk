# currently the only SoC supported; to be moved into Kconfig
SOC_NAME := esp32

COMPONENT_SRCDIRS := $(SOC_NAME)
COMPONENT_ADD_INCLUDEDIRS := $(SOC_NAME)/include include
