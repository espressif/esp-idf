idf_component_register(SRCS "panic.c" "system_api.c" "startup.c" "sleep_modes.c" "system_time.c"
                    INCLUDE_DIRS include
                    PRIV_REQUIRES spi_flash app_update
                                  # requirements due to startup code
                                  nvs_flash pthread app_trace
                    LDFRAGMENTS "linker.lf")

add_subdirectory(port)

# After system initialization, `start_app` (and its other cores variant) is called. 
# This is provided by the user or from another component. Since we can't establish 
# dependency on what we don't know, force linker to not drop the symbol regardless
# of link line order.
target_link_libraries(${COMPONENT_LIB} INTERFACE "-u start_app")

if (NOT CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u start_app_other_cores")
endif()

# Disable stack protection in files which are involved in initialization of that feature
set_source_files_properties(
    startup.c
    PROPERTIES COMPILE_FLAGS
    -fno-stack-protector)
