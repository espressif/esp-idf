ifndef CONFIG_ESP_CONSOLE_USB_CDC
    COMPONENT_OBJEXCLUDE := vfs_cdcacm.o
endif
ifndef CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG
    COMPONENT_OBJEXCLUDE := vfs_usb_serial_jtag.o
endif
