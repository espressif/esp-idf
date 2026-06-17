set(srcs "cap_initiator.c"
         "ccp_server.c"
         "mcp_server.c"
         "vcp_vol_ctlr.c"
         "main.c")

if(CONFIG_BT_BLUEDROID_ENABLED)
    list(APPEND srcs "bluedroid/central.c")
else()
    list(APPEND srcs "nimble/central.c")
endif()

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "."
                       REQUIRES bt nvs_flash)
