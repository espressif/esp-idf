idf_component_register(SRCS "src/jsmn.c"
                    INCLUDE_DIRS "include")

if(CONFIG_JSMN_PARENT_LINKS)
    target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DJSMN_PARENT_LINKS")
endif()

if(CONFIG_JSMN_STRICT)
    target_compile_definitions(${COMPONENT_LIB} PUBLIC "-DJSMN_STRICT")
endif()
