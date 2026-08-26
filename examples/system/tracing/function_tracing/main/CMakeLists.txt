idf_component_register(SRCS "function_tracing_example_main.c"
                    PRIV_REQUIRES ft_demo
                    INCLUDE_DIRS ".")

# Demonstrate file-level instrumentation. It instruments only this source.
if(CONFIG_ESP_TRACE_FUNCTION_TRACE)
    set_source_files_properties(function_tracing_example_main.c PROPERTIES
        COMPILE_OPTIONS "-finstrument-functions")
endif()
