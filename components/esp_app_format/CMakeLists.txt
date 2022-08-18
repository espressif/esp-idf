idf_component_register(SRCS "esp_app_desc.c"
                    INCLUDE_DIRS "include")

# esp_app_desc structure is added as an undefined symbol because otherwise the
# linker will ignore this structure as it has no other files depending on it.
if(NOT BOOTLOADER_BUILD)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u esp_app_desc")
endif()

if(CONFIG_APP_PROJECT_VER_FROM_CONFIG)
    # Ignore current PROJECT_VER (which was set in __project_get_revision()).
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
