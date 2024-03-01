set(srcs)
set(include)
set(priv_includes)
# As CONFIG_SOC_USB_OTG_SUPPORTED comes from Kconfig, it is not evaluated yet
# when components are being registered.
# Thus, always add the (private) requirements, regardless of Kconfig
set(priv_requires driver) # usb_phy driver relies on gpio driver API

if(CONFIG_SOC_USB_OTG_SUPPORTED)
    list(APPEND srcs "hcd_dwc.c"
                     "hub.c"
                     "usb_helpers.c"
                     "usb_host.c"
                     "usb_private.c"
                     "usbh.c"
                     "usb_phy.c")
    list(APPEND include "include")
    list(APPEND priv_includes "private_include")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${include}
                       PRIV_INCLUDE_DIRS ${priv_includes}
                       PRIV_REQUIRES ${priv_requires}
                       )
