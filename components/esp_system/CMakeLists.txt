idf_component_register(SRCS "panic.c" "system_api.c" "startup.c"
                    INCLUDE_DIRS include
                    PRIV_INCLUDE_DIRS private_include
                    PRIV_REQUIRES spi_flash app_update
                                # requirements due to startup code
                                nvs_flash pthread app_trace
                    LDFRAGMENTS "linker.lf")

add_subdirectory(port)

# Rely on user code to define app_main
target_link_libraries(${COMPONENT_LIB} INTERFACE "-u app_main")

if (NOT CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u app_mainX")
endif()