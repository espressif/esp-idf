idf_build_get_property(target IDF_TARGET)


if(BOOTLOADER_BUILD)
    # For bootloader, all we need from esp_common is headers
    idf_component_register(SRCS ${srcs}
                           INCLUDE_DIRS include
                           PRIV_REQUIRES soc)
    set_property(TARGET ${COMPONENT_LIB} APPEND PROPERTY INTERFACE_LINK_LIBRARIES "-Wl,--gc-sections")
else()
    # Regular app build
    list(APPEND srcs "src/esp_err_to_name.c")

    # Note: esp_ipc, esp_pm added as a public requirement to keep compatibility as to be located here.
    idf_component_register(SRCS "${srcs}"
                           INCLUDE_DIRS include
                           REQUIRES ${target} espcoredump esp_timer esp_ipc esp_pm esp_hw_support
                           PRIV_REQUIRES soc)

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
                      tcpip_adapter
                      esp_serial_slave_link
                      esp_netif
                      soc
                      esp-tls
                      esp_https_ota)

    idf_build_get_property(build_components BUILD_COMPONENTS)
    foreach(req ${optional_reqs})
        if(req IN_LIST build_components)
            idf_component_get_property(req_lib ${req} COMPONENT_LIB)
            target_link_libraries(${COMPONENT_LIB} PRIVATE ${req_lib})
        endif()
    endforeach()

endif()

if(CONFIG_IDF_ENV_FPGA)
    # Forces the linker to include fpga stubs from this component
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u esp_common_include_fpga_overrides")
endif()
