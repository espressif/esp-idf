idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(includes "include")
set(srcs)

# USB-DWC related source files and USB FSLS PHY wrapper
if(CONFIG_SOC_USB_OTG_SUPPORTED)
    list(APPEND srcs "usb_dwc_hal.c" "usb_wrap_hal.c" "${target}/usb_dwc_periph.c")
    list(APPEND includes "${target}/include")
endif()

# USB UTMI PHY
if(CONFIG_SOC_USB_UTMI_PHY_NUM GREATER 0)
    list(APPEND srcs "usb_utmi_hal.c")
endif()

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS ${includes}
                    REQUIRES soc hal)
