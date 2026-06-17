idf_component_register(SRCS plugin_nihao.c
                       PRIV_REQUIRES plugins)

# This is equivalent to adding WHOLE_ARCHIVE option to the idf_component_register call above:
idf_component_set_property(${COMPONENT_NAME} WHOLE_ARCHIVE TRUE)
