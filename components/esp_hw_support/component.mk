COMPONENT_SRCDIRS := . port/$(IDF_TARGET)
COMPONENT_ADD_INCLUDEDIRS := . include port/$(IDF_TARGET)/ include/soc port/$(IDF_TARGET)/private_include
COMPONENT_ADD_LDFRAGMENTS := linker.lf

ifdef IS_BOOTLOADER_BUILD
COMPONENT_OBJEXCLUDE += clk_ctrl_os.o mac_addr.o sp_async_memcpy.o intr_alloc.o sleep_modes.o regi2c_ctrl.o
endif

COMPONENT_OBJEXCLUDE += esp_async_memcpy.o
