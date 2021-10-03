COMPONENT_SRCDIRS := . port/$(IDF_TARGET)
COMPONENT_ADD_INCLUDEDIRS := . include port/$(IDF_TARGET)/ include/soc include/soc/${IDF_TARGET} port/$(IDF_TARGET)/private_include
COMPONENT_ADD_LDFRAGMENTS := linker.lf

ifdef IS_BOOTLOADER_BUILD
COMPONENT_OBJEXCLUDE += clk_ctrl_os.o \
						intr_alloc.o \
						sleep_modes.o \
						sleep_gpio.o \
						sleep_mac_bb.o \
						sleep_retention.o \
						esp_async_memcpy.o \
						mac_addr.o \
						regi2c_ctrl.o \
						port/$(IDF_TARGET)/dport_access.o \
						port/$(IDF_TARGET)/cache_sram_mmu.o \
						port/$(IDF_TARGET)/esp_himem.o \
						port/$(IDF_TARGET)/spiram.o \
						port/$(IDF_TARGET)/spiram_psram.o
endif

ifdef CONFIG_IDF_TARGET_ESP32
COMPONENT_OBJEXCLUDE += sleep_retention.o
endif
ifdef CONFIG_IDF_TARGET_ESP32S2
COMPONENT_OBJEXCLUDE += sleep_retention.o
endif

COMPONENT_OBJEXCLUDE += esp_async_memcpy.o
