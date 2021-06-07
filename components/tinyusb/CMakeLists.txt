idf_build_get_property(target IDF_TARGET)
idf_component_register(REQUIRES esp_rom freertos vfs soc)

if(CONFIG_USB_ENABLED)

  if(target STREQUAL "esp32s3")
      set(tusb_mcu "OPT_MCU_ESP32S3")
      set(tusb_family "esp32sx")
  elseif(target STREQUAL "esp32s2")
      set(tusb_mcu "OPT_MCU_ESP32S2")
      set(tusb_family "esp32sx")
  else()
      message("TinyUSB does not support ${target}.")
      return()
  endif()

  ### variables ###
  #################
  set(compile_options
      "-DCFG_TUSB_MCU=${tusb_mcu}"
      "-DCFG_TUSB_DEBUG=${CONFIG_USB_DEBUG_LEVEL}"
      "-Wno-type-limits" # needed for the vanila tinyusb with turned off classes
  )

  idf_component_get_property(FREERTOS_ORIG_INCLUDE_PATH freertos
                             ORIG_INCLUDE_PATH)
  set(includes_private
      # tusb:
      "${COMPONENT_DIR}/tinyusb/hw/bsp/"
      "${COMPONENT_DIR}/tinyusb/src/"
      "${COMPONENT_DIR}/tinyusb/src/device"
      # espressif:
      "${COMPONENT_DIR}/additions/include_private"
      )

  set(includes_public
      # tusb:
      "${FREERTOS_ORIG_INCLUDE_PATH}"
      "${COMPONENT_DIR}/tinyusb/src/"
      "${COMPONENT_DIR}/tinyusb/lib/networking"
      # espressif:
      "${COMPONENT_DIR}/additions/include"
      )
  set(srcs
      # espressif:
      "${COMPONENT_DIR}/additions/src/descriptors_control.c"
      "${COMPONENT_DIR}/additions/src/tinyusb.c"
      "${COMPONENT_DIR}/additions/src/tusb_tasks.c"
      "${COMPONENT_DIR}/additions/src/usb_descriptors.c"
      # tusb:
      "${COMPONENT_DIR}/tinyusb/src/portable/espressif/${tusb_family}/dcd_${tusb_family}.c"
      "${COMPONENT_DIR}/tinyusb/src/class/cdc/cdc_device.c"
      "${COMPONENT_DIR}/tinyusb/src/class/hid/hid_device.c"
      "${COMPONENT_DIR}/tinyusb/src/class/midi/midi_device.c"
      "${COMPONENT_DIR}/tinyusb/src/class/msc/msc_device.c"
      "${COMPONENT_DIR}/tinyusb/src/class/vendor/vendor_device.c"
      "${COMPONENT_DIR}/tinyusb/src/common/tusb_fifo.c"
      "${COMPONENT_DIR}/tinyusb/src/device/usbd_control.c"
      "${COMPONENT_DIR}/tinyusb/src/device/usbd.c"
      "${COMPONENT_DIR}/tinyusb/src/tusb.c")
  # cdc stuff if turned on
  if(CONFIG_USB_CDC_ENABLED)
    list(APPEND srcs
        "${COMPONENT_DIR}/additions/src/cdc.c"
        "${COMPONENT_DIR}/additions/src/tusb_cdc_acm.c"
        "${COMPONENT_DIR}/additions/src/tusb_console.c"
        "${COMPONENT_DIR}/additions/src/vfs_tinyusb.c")
  endif()
  # ethernet over usb
  if(CONFIG_USB_NET_ENABLED)
    list(APPEND srcs
        "${COMPONENT_DIR}/tinyusb/src/class/net/net_device.c"
        "${COMPONENT_DIR}/tinyusb/lib/networking/rndis_reports.c"
        "${COMPONENT_DIR}/additions/src/tusb_cdc_ecm.c")
  endif()

  ### tinyusb lib ###
  ###################
  add_library(tinyusb STATIC ${srcs})
  target_include_directories(
    tinyusb
    PUBLIC ${includes_public}
    PRIVATE ${includes_private})
  target_compile_options(tinyusb PRIVATE ${compile_options})
  target_link_libraries(${COMPONENT_TARGET} INTERFACE tinyusb)

endif()
