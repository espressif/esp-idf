if(BOOTLOADER_BUILD)
  # Bootloader needs SPIUnlock from this file, but doesn't
  # need other parts of this component
  set(COMPONENT_SRCS spi_flash_rom_patch.c)
else()
  set(COMPONENT_SRCDIRS .)
endif()

set(COMPONENT_ADD_INCLUDEDIRS include)

register_component()
