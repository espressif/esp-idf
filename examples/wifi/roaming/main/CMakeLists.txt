# Embed CA, certificate & key directly into binary
idf_component_register(SRCS "roaming_example.c"
                    INCLUDE_DIRS ".")
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
