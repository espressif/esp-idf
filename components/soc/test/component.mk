# currently the only SoC supported; to be moved into Kconfig
SOC_NAME := esp32

COMPONENT_SRCDIRS := ../$(SOC_NAME)/test

COMPONENT_ADD_LDFLAGS = -Wl,--whole-archive -l$(COMPONENT_NAME) -Wl,--no-whole-archive


