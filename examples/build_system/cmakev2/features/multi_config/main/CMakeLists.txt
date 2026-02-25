idf_component_register(SRCS "multi_config_example_main.c"
                       INCLUDE_DIRS ".")

if(CONFIG_EXAMPLE_FUNC_IMPL_DEV)
    target_sources(${COMPONENT_LIB} PRIVATE "func_dev.c")
elseif(CONFIG_EXAMPLE_FUNC_IMPL_PROD)
    target_sources(${COMPONENT_LIB} PRIVATE "func_prod.c")
endif()
