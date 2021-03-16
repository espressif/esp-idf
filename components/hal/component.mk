COMPONENT_SRCDIRS := . esp32
COMPONENT_ADD_INCLUDEDIRS := esp32/include include
COMPONENT_ADD_LDFRAGMENTS += linker.lf

COMPONENT_OBJEXCLUDE += ./spi_slave_hd_hal.o ./spi_flash_hal_gpspi.o ./spi_slave_hd_hal.o ./ds_hal.o ./gdma_hal.o

ifndef CONFIG_ETH_USE_ESP32_EMAC
    COMPONENT_OBJEXCLUDE += esp32/emac_hal.o
endif

ifdef IS_BOOTLOADER_BUILD
    COMPONENT_OBJEXCLUDE += esp32/emac_hal.o
endif
