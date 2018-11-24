set(COMPONENT_SRCS "main.c")
set(COMPONENT_ADD_INCLUDEDIRS ".")

set(COMPONENT_EMBED_TXTFILES
        "certs/cacert.pem"
        "certs/prvtkey.pem")

register_component()
