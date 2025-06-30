set(srcs "main.c")

if(CONFIG_CHERRYUSB_HOST_HID)
    list(APPEND srcs "hid.c")
endif()

if(CONFIG_CHERRYUSB_HOST_CDC_ACM OR CONFIG_CHERRYUSB_HOST_FTDI OR CONFIG_CHERRYUSB_HOST_CH34X OR
   CONFIG_CHERRYUSB_HOST_CP210X OR CONFIG_CHERRYUSB_HOST_PL2303)
    list(APPEND srcs "cdc_acm.c")
endif()

idf_component_register(SRCS ${srcs}
    INCLUDE_DIRS "."
)

if(CONFIG_CHERRYUSB_HOST_HID)
    # Make sure the definitions in hid.c are linked correctly
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u usbh_hid_run_real")
endif()

if(CONFIG_CHERRYUSB_HOST_CDC_ACM)
    # Make sure the definitions in hid.c are linked correctly
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u usbh_cdc_acm_run_real")
endif()

if(CONFIG_CHERRYUSB_HOST_FTDI)
    # Make sure the definitions in hid.c are linked correctly
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u usbh_ftdi_run_real")
endif()

if(CONFIG_CHERRYUSB_HOST_CH34X)
    # Make sure the definitions in hid.c are linked correctly
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u usbh_ch34x_run_real")
endif()

if(CONFIG_CHERRYUSB_HOST_CP210X)
    # Make sure the definitions in hid.c are linked correctly
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u usbh_cp210x_run_real")
endif()

if(CONFIG_CHERRYUSB_HOST_PL2303)
    # Make sure the definitions in hid.c are linked correctly
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u usbh_pl2303_run_real")
endif()
