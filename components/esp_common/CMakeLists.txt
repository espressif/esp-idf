idf_build_get_property(target IDF_TARGET)

if(BOOTLOADER_BUILD)
    # For bootloader, all we need from esp_common is headers
    idf_component_register(INCLUDE_DIRS include)
    set_property(TARGET ${COMPONENT_LIB} APPEND PROPERTY INTERFACE_LINK_LIBRARIES "-Wl,--gc-sections")
else()
    # Regular app build
    set(srcs "src/brownout.c"
             "src/esp_err.c"
             "src/dbg_stubs.c"
             "src/esp_err_to_name.c"
             "src/freertos_hooks.c"
             "src/mac_addr.c"
             "src/pm_locks.c"
             "src/stack_check.c"
             "src/task_wdt.c"
             "src/int_wdt.c")

    # Note: esp_ipc added as a public requirement to keep compatibility as to be located here.
    idf_component_register(SRCS "${srcs}"
                           INCLUDE_DIRS include
                           REQUIRES ${target} espcoredump esp_timer esp_ipc
                           PRIV_REQUIRES soc
                           LDFRAGMENTS "linker.lf")

    set_source_files_properties(
        "src/stack_check.c"
        PROPERTIES COMPILE_FLAGS
        -fno-stack-protector)
    set_property(TARGET ${COMPONENT_LIB} APPEND PROPERTY LINK_LIBRARIES "-Wl,--gc-sections")
    set_property(TARGET ${COMPONENT_LIB} APPEND PROPERTY INTERFACE_LINK_LIBRARIES "-Wl,--gc-sections")
    set_property(TARGET ${COMPONENT_LIB} APPEND PROPERTY LINK_INTERFACE_MULTIPLICITY 4)

    # List of components needed for the error codes list
    set(optional_reqs ulp
                      efuse
                      esp_http_client
                      esp_http_server
                      bootloader_support
                      nvs_flash
                      esp_wifi
                      app_update
                      lwip
                      spi_flash
                      wpa_supplicant
                      tcpip_adapter)

    idf_build_get_property(build_components BUILD_COMPONENTS)
    foreach(req ${optional_reqs})
        if(req IN_LIST build_components)
            idf_component_get_property(req_lib ${req} COMPONENT_LIB)
            target_link_libraries(${COMPONENT_LIB} PRIVATE ${req_lib})
        endif()
    endforeach()

endif()
