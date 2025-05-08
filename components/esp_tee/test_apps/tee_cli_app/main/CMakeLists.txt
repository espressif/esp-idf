set(srcs "tee_srv_ota.c"
         "tee_srv_sec_str.c"
         "tee_cmd_wifi.c"
         "app_main.c")

if(CONFIG_SECURE_TEE_ATTESTATION)
    list(APPEND srcs "tee_srv_att.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ".")
