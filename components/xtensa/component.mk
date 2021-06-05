COMPONENT_ADD_INCLUDEDIRS := include esp32/include

COMPONENT_ADD_LDFLAGS += $(COMPONENT_PATH)/esp32/libxt_hal.a

COMPONENT_ADD_LDFRAGMENTS += linker.lf

COMPONENT_SRCDIRS := . esp32

ifdef IS_BOOTLOADER_BUILD
	COMPONENT_OBJEXCLUDE := xtensa_intr.o xtensa_intr_asm.o
endif
