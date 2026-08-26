set(srcs "main.c"
         "cap_handover_proc.c"
         "cap_handover_unicast.c"
         "cap_handover_broadcast.c"
         "cap_handover_tx.c"
         "cap_handover_diag.c")

if(CONFIG_BT_BLUEDROID_ENABLED)
    list(APPEND srcs "bluedroid/central.c"
                     "bluedroid/adv.c")
else()
    list(APPEND srcs "nimble/central.c"
                     "nimble/adv.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "."
                       REQUIRES bt nvs_flash)
