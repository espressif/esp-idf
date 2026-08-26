set(srcs "bap_unicast_sr.c"
         "button.c"
         "ccp_call_ctrl.c"
         "mcp_ctlr.c"
         "tmap_ct_umr.c"
         "vcp_vol_renderer.c"
         "main.c")

if(CONFIG_EXAMPLE_TMAP_PER_DUO)
    list(APPEND srcs "csip_set_member.c")
endif()

if(CONFIG_BT_BLUEDROID_ENABLED)
    list(APPEND srcs "bluedroid/peripheral.c")
else()
    list(APPEND srcs "nimble/peripheral.c")
endif()

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "."
                       REQUIRES bt nvs_flash)
