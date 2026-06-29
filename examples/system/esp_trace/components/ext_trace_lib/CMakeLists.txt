set(src_dirs
    "src"
)

set(include_dirs
    "include"
)

set(priv_requires
    "esp_trace"
)

if(CONFIG_ESP_TRACE_LIB_EXTERNAL)
    idf_component_register(SRC_DIRS ${src_dirs}
                        INCLUDE_DIRS ${include_dirs}
                        PRIV_REQUIRES ${priv_requires}
                        WHOLE_ARCHIVE TRUE)

    idf_component_get_property(freertos_lib freertos COMPONENT_LIB)
    target_include_directories(${freertos_lib} INTERFACE ${include_dirs})
else()
    idf_component_register(PRIV_REQUIRES ${priv_requires})
endif()
