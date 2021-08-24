#
# Component Makefile
#

COMPONENT_SRCDIRS := src
ifdef CONFIG_ESP_IPC_ISR_ENABLE
    COMPONENT_SRCDIRS += src/esp_ipc_isr
endif
COMPONENT_ADD_INCLUDEDIRS := include
