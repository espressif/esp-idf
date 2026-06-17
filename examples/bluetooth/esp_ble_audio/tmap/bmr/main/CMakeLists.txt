set(srcs "bap_broadcast_sink.c"
         "vcp_vol_renderer.c"
         "main.c")

if(CONFIG_BT_BLUEDROID_ENABLED)
    list(APPEND srcs "bluedroid/scan.c")
else()
    list(APPEND srcs "nimble/scan.c")
endif()

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "."
                       REQUIRES bt nvs_flash)
