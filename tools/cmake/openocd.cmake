# __get_openocd_options
# Prepares openocd default options for the target.
function(__get_openocd_options openocd_option_var)
    if(CONFIG_IDF_TARGET STREQUAL "esp32")
        set(${openocd_option_var} "-f board/esp32-wrover-kit-3.3v.cfg" PARENT_SCOPE)
    elseif(CONFIG_IDF_TARGET STREQUAL "esp32s2")
        set(${openocd_option_var} "-f board/esp32s2-kaluga-1.cfg" PARENT_SCOPE)
    elseif(CONFIG_SOC_USB_SERIAL_JTAG_SUPPORTED)
        set(${openocd_option_var} "-f board/${CONFIG_IDF_TARGET}-builtin.cfg" PARENT_SCOPE)
    else()
        set(${openocd_option_var} "-f board/${CONFIG_IDF_TARGET}-ftdi.cfg" PARENT_SCOPE)
    endif()
endfunction()
