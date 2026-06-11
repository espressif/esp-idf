idf_build_get_property(target IDF_TARGET)
idf_build_get_property(non_os_build NON_OS_BUILD)

if(IDF_BUILD_V2 AND CONFIG_ESP_COREDUMP_ENABLE
        AND NOT non_os_build
        AND NOT "${target}" STREQUAL "linux")
    # This ensures that the component is added to the build when coredump is enabled,
    # even if no other component depends on it.
    # This allows users to simply enable it in menuconfig without needing to modify their component dependencies.
    idf_project_add_default_build_component(espcoredump)
endif()
