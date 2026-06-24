set(srcs "cap_initiator_tx.c"
         "main.c")

if(CONFIG_EXAMPLE_UNICAST)
    list(APPEND srcs "cap_initiator_unicast.c")
    if(CONFIG_BT_BLUEDROID_ENABLED)
        list(APPEND srcs "bluedroid/central.c")
    else()
        list(APPEND srcs "nimble/central.c")
    endif()
endif()

if(CONFIG_EXAMPLE_BROADCAST)
    list(APPEND srcs "cap_initiator_broadcast.c")
    if(CONFIG_BT_BLUEDROID_ENABLED)
        list(APPEND srcs "bluedroid/adv.c")
    else()
        list(APPEND srcs "nimble/adv.c")
    endif()
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "."
                       REQUIRES bt nvs_flash)
