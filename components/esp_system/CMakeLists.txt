if(BOOTLOADER_BUILD)
    # Bootloader relies on some Kconfig options defined in esp_system.
    idf_component_register()
    return()
endif()

idf_component_register(SRCS "intr_alloc.c"
                            "esp_async_memcpy.c"
                            "panic.c"
                            "system_api.c"
                            "startup.c"
                            "system_time.c"
                            "sleep_modes.c"
                    INCLUDE_DIRS include
                    PRIV_REQUIRES spi_flash
                                  # [refactor-todo] requirements due to init code,
                                  # should be removable once using component init functions
                                  # link-time registration is used.
                                  esp_pm app_update nvs_flash pthread app_trace esp_gdbstub
                    LDFRAGMENTS "linker.lf")

add_subdirectory(port)

# After system initialization, `start_app` (and its other cores variant) is called.
# This is provided by the user or from another component. Since we can't establish
# dependency on what we don't know, force linker to not drop the symbol regardless
# of link line order.
target_link_libraries(${COMPONENT_LIB} INTERFACE "-u start_app")

if(NOT CONFIG_ESP_SYSTEM_SINGLE_CORE_MODE)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u start_app_other_cores")
endif()

# Disable stack protection in files which are involved in initialization of that feature
set_source_files_properties(
    startup.c
    PROPERTIES COMPILE_FLAGS
    -fno-stack-protector)

if(NOT CMAKE_BUILD_EARLY_EXPANSION)
    set_source_files_properties("${CMAKE_CURRENT_LIST_DIR}/sleep_modes.c" PROPERTIES
                                COMPILE_FLAGS "-fno-jump-tables -fno-tree-switch-conversion")
endif()
