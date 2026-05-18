set(srcs "cap_initiator.c"
         "main.c")

if(CONFIG_BT_BLUEDROID_ENABLED)
    list(APPEND srcs "bluedroid/adv.c")
else()
    list(APPEND srcs "nimble/adv.c")
endif()

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "."
                       REQUIRES bt nvs_flash)
