#
# Component Makefile
#

ifdef CONFIG_FREERTOS_DEBUG_OCDAWARE
	COMPONENT_ADD_LDFLAGS += -Wl,--undefined=uxTopUsedPriority
endif

COMPONENT_ADD_INCLUDEDIRS := include include/esp_additions include/esp_additions/freertos port/xtensa/include
COMPONENT_PRIV_INCLUDEDIRS := include/esp_additions include/esp_additions/freertos include/freertos port/xtensa/include/freertos port/xtensa port/priv_include .
COMPONENT_SRCDIRS += port port/xtensa esp_additions/

ifndef CONFIG_ESP32_IRAM_AS_8BIT_ACCESSIBLE_MEMORY
	COMPONENT_OBJEXCLUDE := xtensa/xtensa_loadstore_handler.o
endif

tasks.o event_groups.o timers.o queue.o: CFLAGS += -D_ESP_FREERTOS_INTERNAL
COMPONENT_ADD_LDFRAGMENTS += linker.lf
