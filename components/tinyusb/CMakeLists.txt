idf_component_register(REQUIRES esp_rom freertos soc driver)

if(CONFIG_USB_ENABLED)
    idf_component_get_property( FREERTOS_ORIG_INCLUDE_PATH freertos ORIG_INCLUDE_PATH)

    target_compile_options(${COMPONENT_TARGET} INTERFACE
        "-DCFG_TUSB_MCU=OPT_MCU_ESP32_S2"
    )

    target_include_directories(${COMPONENT_TARGET} INTERFACE
        "${FREERTOS_ORIG_INCLUDE_PATH}"
        # espressif:
        "${COMPONENT_DIR}/port/esp32s2/include/"
        "${COMPONENT_DIR}/port/common/include"
        # tusb:
        "${COMPONENT_DIR}/tinyusb/hw/bsp/"
        "${COMPONENT_DIR}/tinyusb/src/"
        "${COMPONENT_DIR}/tinyusb/src/device"
    )

    target_sources(${COMPONENT_TARGET} INTERFACE
        # espressif:
        "${COMPONENT_DIR}/port/common/src/descriptors_control.c"
        "${COMPONENT_DIR}/port/common/src/usb_descriptors.c"
        "${COMPONENT_DIR}/port/common/src/usbd.c"
        "${COMPONENT_DIR}/port/esp32s2/src/device_controller_driver.c"
        "${COMPONENT_DIR}/port/esp32s2/src/tinyusb.c"
        # tusb:
        "${COMPONENT_DIR}/tinyusb/src/common/tusb_fifo.c"
        "${COMPONENT_DIR}/tinyusb/src/device/usbd_control.c"
        "${COMPONENT_DIR}/tinyusb/src/class/msc/msc_device.c"
        "${COMPONENT_DIR}/tinyusb/src/class/cdc/cdc_device.c"
        "${COMPONENT_DIR}/tinyusb/src/class/hid/hid_device.c"
        "${COMPONENT_DIR}/tinyusb/src/class/midi/midi_device.c"
        "${COMPONENT_DIR}/tinyusb/src/tusb.c"
        )
endif()
