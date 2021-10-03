set(srcs)
set(include)
set(priv_include)
set(priv_require)

if(CONFIG_USB_OTG_SUPPORTED)
    list(APPEND srcs "hcd.c"
                     "hub.c"
                     "usb_helpers.c"
                     "usb_host.c"
                     "usb_private.c"
                     "usbh.c"
                     "usb_phy.c")
    list(APPEND include "include")
    list(APPEND priv_include "private_include")
    list(APPEND priv_require "hal" "driver")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${include}
                       PRIV_INCLUDE_DIRS ${priv_include}
                       PRIV_REQUIRES ${priv_require}
                       )
