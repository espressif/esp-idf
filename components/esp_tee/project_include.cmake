idf_build_get_property(idf_path IDF_PATH)
idf_build_get_property(idf_target IDF_TARGET)
idf_build_get_property(build_dir BUILD_DIR)
idf_build_get_property(sdkconfig SDKCONFIG)
idf_build_get_property(python PYTHON)
idf_build_get_property(extra_cmake_args EXTRA_CMAKE_ARGS)
idf_build_get_property(project_dir PROJECT_DIR)
idf_build_get_property(non_os_build NON_OS_BUILD)
idf_build_get_property(custom_secure_service_dir CUSTOM_SECURE_SERVICE_COMPONENT_DIR)
idf_build_get_property(custom_secure_service_component CUSTOM_SECURE_SERVICE_COMPONENT)

# Under build system v2, partition_table/project_include.cmake may run after
# this file, so PARTITION_TABLE_BIN_PATH is not set yet. Read it as a generator
# expression, which externalproject_add resolves at generate time. The value is
# only used as a CMAKE_ARGS entry below, so the same read works for v1 as well.
idf_build_get_property(partition_table_bin PARTITION_TABLE_BIN_PATH GENERATOR_EXPRESSION)

if(NOT CONFIG_SECURE_ENABLE_TEE OR non_os_build)
    return()
endif()

if(CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES)
         get_filename_component(secure_boot_signing_key
         "${CONFIG_SECURE_BOOT_SIGNING_KEY}"
         ABSOLUTE BASE_DIR "${project_dir}")

         set(SECURE_BOOT_SIGNING_KEY ${secure_boot_signing_key})
         set(sign_key_arg "-DSECURE_BOOT_SIGNING_KEY=${secure_boot_signing_key}")
else()
         set(sign_key_arg)
endif()

set(TEE_BUILD_DIR "${build_dir}/esp_tee")
set(tee_binary_files
    "${TEE_BUILD_DIR}/esp_tee.elf"
    "${TEE_BUILD_DIR}/esp_tee.bin"
    "${TEE_BUILD_DIR}/esp_tee.map"
    )

# Force a boolean CONFIG_ option to y/n in the given sdkconfig content variable
function(__esp_tee_sdkconfig_set content_var option value)
    if(value STREQUAL "y")
        set(line "${option}=y")
    elseif(value STREQUAL "n")
        set(line "# ${option} is not set")
    else()
        message(FATAL_ERROR "__esp_tee_sdkconfig_set: unsupported value '${value}' for ${option}")
    endif()
    # Drop every existing line of the option (either spelling), then append the forced one
    set(content "\n${${content_var}}")
    string(REGEX REPLACE "\n(# )?${option}( is not set|=[^\n]*)" "" content "${content}")
    string(REGEX REPLACE "^\n" "" content "${content}")
    if(NOT content MATCHES "\n$")
        string(APPEND content "\n")
    endif()
    string(APPEND content "${line}\n")
    set(${content_var} "${content}" PARENT_SCOPE)
endfunction()

set(esp_tee_sdkconfig "${CMAKE_CURRENT_BINARY_DIR}/sdkconfig.esp_tee")
configure_file("${sdkconfig}" "${esp_tee_sdkconfig}" COPYONLY)

file(READ "${esp_tee_sdkconfig}" content)

# Override LibC for ESP-TEE if needed
unset(REE_LIBC)
unset(TEE_LIBC)

foreach(libc NEWLIB PICOLIBC)
    if(content MATCHES "CONFIG_LIBC_${libc}=y")
        set(REE_LIBC ${libc})
    endif()

    if(content MATCHES "CONFIG_SECURE_TEE_LIBC_${libc}=y")
        set(TEE_LIBC ${libc})
    endif()
endforeach()

if(REE_LIBC AND TEE_LIBC AND NOT REE_LIBC STREQUAL TEE_LIBC)
    __esp_tee_sdkconfig_set(content "CONFIG_LIBC_${REE_LIBC}" n)
    __esp_tee_sdkconfig_set(content "CONFIG_LIBC_${TEE_LIBC}" y)
endif()

# The TEE binary must always be reproducible, irrespective of the REE app config
__esp_tee_sdkconfig_set(content "CONFIG_APP_REPRODUCIBLE_BUILD" y)
# NOTE: The REE app must not be able to rename the TEE image
__esp_tee_sdkconfig_set(content "CONFIG_APP_PROJECT_VER_FROM_CONFIG" n)

file(WRITE "${esp_tee_sdkconfig}" "${content}")

set(secure_service_headers_dir "${CMAKE_CURRENT_BINARY_DIR}/secure_service_headers")
make_directory(${secure_service_headers_dir})
idf_build_set_property(SECURE_SERVICE_HEADERS_DIR "${secure_service_headers_dir}")
include_directories("${secure_service_headers_dir}")

externalproject_add(esp_tee
    SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/subproject"
    BINARY_DIR "${TEE_BUILD_DIR}"
    CMAKE_ARGS  -DSDKCONFIG=${esp_tee_sdkconfig} -DIDF_PATH=${idf_path} -DIDF_TARGET=${idf_target}
                -DCUSTOM_SECURE_SERVICE_COMPONENT=${custom_secure_service_component}
                -DCUSTOM_SECURE_SERVICE_COMPONENT_DIR=${custom_secure_service_dir}
                -DSECURE_SERVICE_HEADERS_DIR=${secure_service_headers_dir}
                -DPARTITION_TABLE_BIN_PATH=${partition_table_bin}
                -DIDF_BUILD_V2=${IDF_BUILD_V2}
                -DCCACHE_ENABLE=${CCACHE_ENABLE}
                ${extra_cmake_args} ${sign_key_arg}
    INSTALL_COMMAND ""
    BUILD_ALWAYS 1  # no easy way around this...
    USES_TERMINAL_CONFIGURE TRUE
    USES_TERMINAL_BUILD TRUE
    BUILD_BYPRODUCTS ${tee_binary_files}
    )

set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" APPEND PROPERTY
    ADDITIONAL_MAKE_CLEAN_FILES
    ${tee_binary_files})
