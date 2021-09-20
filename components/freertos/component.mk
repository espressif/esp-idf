#
# Component Makefile
#

ifdef CONFIG_FREERTOS_DEBUG_OCDAWARE
	COMPONENT_ADD_LDFLAGS += -Wl,--undefined=uxTopUsedPriority
endif

COMPONENT_ADD_INCLUDEDIRS := FreeRTOS-Kernel/include esp_additions/include esp_additions/include/freertos FreeRTOS-Kernel/portable/xtensa/include
COMPONENT_PRIV_INCLUDEDIRS := esp_additions/include esp_additions/include/freertos FreeRTOS-Kernel/include/freertos FreeRTOS-Kernel/portable/xtensa/include/freertos FreeRTOS-Kernel/portable/xtensa FreeRTOS-Kernel/portable/priv_include .
COMPONENT_SRCDIRS += FreeRTOS-Kernel FreeRTOS-Kernel/portable FreeRTOS-Kernel/portable/xtensa esp_additions/

ifndef CONFIG_ESP32_IRAM_AS_8BIT_ACCESSIBLE_MEMORY
	COMPONENT_OBJEXCLUDE := xtensa/xtensa_loadstore_handler.o
endif

tasks.o event_groups.o timers.o queue.o: CFLAGS += -D_ESP_FREERTOS_INTERNAL
COMPONENT_ADD_LDFRAGMENTS += linker.lf
