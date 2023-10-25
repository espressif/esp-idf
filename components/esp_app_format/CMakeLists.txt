idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

if(NOT BOOTLOADER_BUILD)
    set(src "esp_app_desc.c")
else()
    set(src "")
endif()
idf_component_register(SRCS ${src}
                    INCLUDE_DIRS "include")

if(NOT BOOTLOADER_BUILD)
    # esp_app_desc structure is added as an undefined symbol because otherwise the
    # linker will ignore this structure as it has no other files depending on it.
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u esp_app_desc")

    if(CONFIG_APP_PROJECT_VER_FROM_CONFIG)
        # Ignore current PROJECT_VER (which was set in project.cmake)
        # Gets the version from the CONFIG_APP_PROJECT_VER.
        idf_build_set_property(PROJECT_VER "${CONFIG_APP_PROJECT_VER}")
    endif()

    # cut PROJECT_VER and PROJECT_NAME to required 32 characters.
    idf_build_get_property(project_ver PROJECT_VER)
    idf_build_get_property(project_name PROJECT_NAME)
    string(SUBSTRING "${project_ver}" 0 31 PROJECT_VER_CUT)
    string(SUBSTRING "${project_name}" 0 31 PROJECT_NAME_CUT)
    message(STATUS "App \"${PROJECT_NAME_CUT}\" version: ${PROJECT_VER_CUT}")

    set_source_files_properties(
        SOURCE "esp_app_desc.c"
        PROPERTIES COMPILE_DEFINITIONS
        "PROJECT_VER=\"${PROJECT_VER_CUT}\"; PROJECT_NAME=\"${PROJECT_NAME_CUT}\"")
endif()
