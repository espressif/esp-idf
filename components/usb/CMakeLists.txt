set(srcs)
set(include)
set(priv_include)
# As CONFIG_USB_OTG_SUPPORTED comes from Kconfig, it is not evaluated yet
# when components are being registered.
# Thus, always add the (private) requirements, regardless of Kconfig
# [refactor-todo]: requires "driver" because "periph_ctrl.h" is used in "hcd.c"
set(priv_require hal driver)

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
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${include}
                       PRIV_INCLUDE_DIRS ${priv_include}
                       PRIV_REQUIRES ${priv_require}
                       )
