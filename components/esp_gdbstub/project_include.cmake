idf_build_get_property(target IDF_TARGET)
idf_build_get_property(non_os_build NON_OS_BUILD)

if(IDF_BUILD_V2 AND NOT non_os_build AND NOT "${target}" STREQUAL "linux")
    if(CONFIG_ESP_SYSTEM_GDBSTUB_RUNTIME OR CONFIG_ESP_SYSTEM_PANIC_GDBSTUB)
        idf_project_add_default_build_component(esp_gdbstub)
    endif()
endif()
