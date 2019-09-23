set(COMPONENT_SRCS "app_main.c" "esp_local_ctrl_service.c")
set(COMPONENT_ADD_INCLUDEDIRS ".")

set(COMPONENT_EMBED_TXTFILES
        "certs/cacert.pem"
        "certs/prvtkey.pem")

register_component()
