#
# Component Makefile
#
COMPONENT_ADD_INCLUDEDIRS := include
COMPONENT_PRIV_INCLUDEDIRS := private_include
COMPONENT_SRCDIRS := .
COMPONENT_ADD_LDFRAGMENTS := linker.lf

ifdef CONFIG_ESP_EVENT_LOOP_PROFILING
	PROFILING_ENABLED := 1
else
	PROFILING_ENABLED := 0
endif

ifeq ($(and $(GCC_NOT_5_2_0),$(PROFILING_ENABLED)), 1)
# uses C11 atomic feature
esp_event.o: CFLAGS += -std=gnu11
endif
