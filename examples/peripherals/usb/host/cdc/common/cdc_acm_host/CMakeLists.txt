set(srcs)
set(include)
# As CONFIG_USB_OTG_SUPPORTED comes from Kconfig, it is not evaluated yet
# when components are being registered.
set(require usb)

if(CONFIG_USB_OTG_SUPPORTED)
    list(APPEND srcs "cdc_acm_host.c")
    list(APPEND include "include")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${include}
                       REQUIRES ${require}
                       )
