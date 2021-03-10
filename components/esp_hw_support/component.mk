COMPONENT_SRCDIRS := . port/$(IDF_TARGET)
COMPONENT_ADD_INCLUDEDIRS := . include include/soc port/$(IDF_TARGET)/private_include
COMPONENT_ADD_LDFRAGMENTS := linker.lf

port/$(IDF_TARGET)/rtc_clk.o: CFLAGS += -fno-jump-tables -fno-tree-switch-conversion

ifdef IS_BOOTLOADER_BUILD
COMPONENT_OBJEXCLUDE += clk_ctrl_os.o mac_addr.o esp_async_memcpy.o intr_alloc.o sleep_modes.o
endif

COMPONENT_OBJEXCLUDE += esp_async_memcpy.o
