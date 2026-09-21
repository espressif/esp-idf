set(ESP_BLOCKDEV_COMPONENT_DIR ${CMAKE_CURRENT_LIST_DIR})

# Skip bootloader builds — ioctl checking is only relevant for the app.
if(BOOTLOADER_BUILD)
    return()
endif()

# Define the callback function that performs the actual check.
function(__esp_blockdev_ioctl_check_post_elf target)
    idf_build_get_property(python PYTHON)
    idf_build_get_property(idf_path IDF_PATH)
    idf_build_get_property(ioctl_def_files ESP_BLOCKDEV_IOCTL_DEF_FILES)

    if(NOT ioctl_def_files)
        return()
    endif()

    # On macOS, Apple Clang needs the SDK sysroot to resolve system headers
    # (e.g. <stdio.h> via esp_err.h) during preprocessing.
    set(ioctl_compiler_flags)
    if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Darwin" AND CMAKE_OSX_SYSROOT)
        # Use --compiler-flag=VALUE so argparse does not treat -isysroot as an option.
        list(APPEND ioctl_compiler_flags "--compiler-flag=-isysroot${CMAKE_OSX_SYSROOT}")
    endif()

    add_custom_command(
        TARGET ${target}
        POST_BUILD
        COMMAND ${python}
                "${ESP_BLOCKDEV_COMPONENT_DIR}/tools/check_ioctl_overlap.py"
                --compiler ${CMAKE_C_COMPILER}
                ${ioctl_compiler_flags}
                --include-dir "${ESP_BLOCKDEV_COMPONENT_DIR}/include"
                --include-dir "${idf_path}/components/esp_common/include"
                --include-dir "${CMAKE_BINARY_DIR}/config"
                --files ${ioctl_def_files}
        COMMENT "Checking ioctl definition overlaps"
        VERBATIM
    )
endfunction()

# Register with the build system. CMakev2 provides a proper build-event API;
# CMakev1 uses cmake_language(DEFER) as a fallback.
if(COMMAND idf_component_register_build_event_callback)
    # cmakev2: use the official callback mechanism
    idf_component_register_build_event_callback(
        EVENT POST_ELF
        CALLBACK __esp_blockdev_ioctl_check_post_elf
    )
else()
    # cmakev1: defer until the ELF target exists
    cmake_language(DEFER DIRECTORY ${CMAKE_SOURCE_DIR} CALL __esp_blockdev_ioctl_check_deferred)
endif()

function(__esp_blockdev_ioctl_check_deferred)
    set(project_elf ${CMAKE_PROJECT_NAME}.elf)
    if(NOT TARGET ${project_elf})
        return()
    endif()
    __esp_blockdev_ioctl_check_post_elf(${project_elf})
endfunction()
