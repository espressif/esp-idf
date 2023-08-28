idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

idf_component_register(SRCS "esp_bootloader_desc.c"
                       INCLUDE_DIRS "include")

if(BOOTLOADER_BUILD)
    # esp_bootloader_desc structure is added as an undefined symbol because otherwise the
    # linker will ignore this structure as it has no other files depending on it.
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u esp_bootloader_desc")

    idf_build_get_property(project_name PROJECT_NAME)
    message(STATUS "Bootloader project name: \"${project_name}\" version: ${CONFIG_BOOTLOADER_PROJECT_VER}")
endif()
