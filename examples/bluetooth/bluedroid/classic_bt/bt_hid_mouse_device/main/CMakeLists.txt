#set(COMPONENT_SRCS "main.c")
#set(COMPONENT_ADD_INCLUDEDIRS "")

#register_component()

idf_component_register(SRCS "main.c"
                    PRIV_REQUIRES bt nvs_flash
                    INCLUDE_DIRS ".")
