COMPONENT_SRCDIRS := . esp32
COMPONENT_ADD_INCLUDEDIRS := esp32/include include platform_port/include
COMPONENT_ADD_LDFRAGMENTS += linker.lf

COMPONENT_OBJEXCLUDE += ./spi_slave_hd_hal.o ./spi_flash_hal_gpspi.o ./spi_slave_hd_hal.o ./ds_hal.o ./gdma_hal.o ./lcd_hal.o ./systimer_hal.o ./usb_hal.o ./usb_dwc_hal.o ./usb_phy_hal.o ./xt_wdt_hal.o

ifndef CONFIG_ETH_USE_ESP32_EMAC
    COMPONENT_OBJEXCLUDE += ./emac_hal.o
endif

ifdef IS_BOOTLOADER_BUILD
    COMPONENT_OBJEXCLUDE += ./emac_hal.o
endif
