idf_build_get_property(target IDF_TARGET)
idf_build_get_property(idf_ver_def IDF_VER)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

if(BOOTLOADER_BUILD)
    set(srcs "esp_bootloader_desc.c")
else()
    set(srcs "")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "include")

set_source_files_properties(
    SOURCE "esp_bootloader_desc.c" PROPERTIES COMPILE_DEFINITIONS
    "IDF_VER_DEF=\"${idf_ver_def}\"")

if(BOOTLOADER_BUILD)
    # esp_bootloader_desc structure is added as an undefined symbol because otherwise the
    # linker will ignore this structure as it has no other files depending on it.
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u esp_bootloader_desc")

    idf_build_get_property(project_name PROJECT_NAME)
    message(STATUS "Bootloader project name: \"${project_name}\" version: ${CONFIG_BOOTLOADER_PROJECT_VER}")
endif()
