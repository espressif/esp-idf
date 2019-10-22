#
#Component Makefile
#

COMPONENT_SRCDIRS += esp32
COMPONENT_ADD_LDFLAGS = -Wl,--whole-archive -l$(COMPONENT_NAME) -Wl,--no-whole-archive

ifdef CONFIG_SPI_FLASH_USE_LEGACY_IMPL
	COMPONENT_OBJEXCLUDE += test_esp_flash.o test_partition_ext.o
endif
