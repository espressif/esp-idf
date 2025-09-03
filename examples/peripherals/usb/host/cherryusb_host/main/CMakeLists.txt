set(srcs "main.c")

if(CONFIG_CHERRYUSB_HOST_HID)
    list(APPEND srcs "hid.c")
endif()

if(CONFIG_CHERRYUSB_HOST_CDC_ACM OR CONFIG_CHERRYUSB_HOST_FTDI OR CONFIG_CHERRYUSB_HOST_CH34X OR
   CONFIG_CHERRYUSB_HOST_CP210X OR CONFIG_CHERRYUSB_HOST_PL2303)
    list(APPEND srcs "cdc_acm.c")
    set(CHERRYUSB_HOST_CDC_ACM_INCLUDE TRUE)
endif()

if(CONFIG_CHERRYUSB_HOST_MSC)
    list(APPEND srcs "msc.c")
endif()

idf_component_register(SRCS ${srcs}
    INCLUDE_DIRS "."
    PRIV_REQUIRES esp_timer fatfs esp_ringbuf
)

if(CONFIG_CHERRYUSB_HOST_HID)
    # Make sure the definitions in hid.c are linked correctly
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u ld_include_hid")
endif()

if(CHERRYUSB_HOST_CDC_ACM_INCLUDE)
    # Make sure the definitions in cdc_acm.c are linked correctly
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u ld_include_cdc_acm")
endif()

if(CONFIG_CHERRYUSB_HOST_MSC)
    # Make sure the definitions in msc.c are linked correctly
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u ld_include_msc")
endif()
