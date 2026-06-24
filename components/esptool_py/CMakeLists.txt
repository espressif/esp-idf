idf_build_get_property(target IDF_TARGET)
idf_build_get_property(esp_tee_build ESP_TEE_BUILD)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

if(esp_tee_build)
    idf_component_register()
    return()
endif()

idf_component_register(REQUIRES bootloader PRIV_REQUIRES partition_table)
