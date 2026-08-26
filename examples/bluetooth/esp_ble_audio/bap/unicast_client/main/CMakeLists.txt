set(srcs "stream_tx.c"
         "main.c")

if(CONFIG_BT_BLUEDROID_ENABLED)
    list(APPEND srcs "bluedroid/central.c")
else()
    list(APPEND srcs "nimble/central.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "."
                       REQUIRES bt nvs_flash)
