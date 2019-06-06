COMPONENT_ADD_INCLUDEDIRS := include
COMPONENT_SRCDIRS := . $(IDF_TARGET)
COMPONENT_PRIV_INCLUDEDIRS := .
COMPONENT_ADD_LDFRAGMENTS += linker.lf

ifdef IS_BOOTLOADER_BUILD
# Bootloader needs updated SPIUnlock from this file
COMPONENT_OBJS := spi_flash_rom_patch.o
endif
