idf_build_get_property(idf_path IDF_PATH)
idf_build_get_property(idf_target IDF_TARGET)
idf_build_get_property(build_dir BUILD_DIR)
idf_build_get_property(sdkconfig SDKCONFIG)
idf_build_get_property(python PYTHON)
idf_build_get_property(extra_cmake_args EXTRA_CMAKE_ARGS)
idf_build_get_property(project_dir PROJECT_DIR)
idf_build_get_property(non_os_build NON_OS_BUILD)
idf_build_get_property(config_dir CONFIG_DIR)
idf_build_get_property(custom_secure_service_dir CUSTOM_SECURE_SERVICE_COMPONENT_DIR)
idf_build_get_property(custom_secure_service_component CUSTOM_SECURE_SERVICE_COMPONENT)


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

externalproject_add(esp_tee
    SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/subproject"
    BINARY_DIR "${TEE_BUILD_DIR}"
    CMAKE_ARGS  -DSDKCONFIG=${sdkconfig} -DIDF_PATH=${idf_path} -DIDF_TARGET=${idf_target}
    -DCONFIG_DIR=${config_dir} -DCUSTOM_SECURE_SERVICE_COMPONENT=${custom_secure_service_component}
    -DCUSTOM_SECURE_SERVICE_COMPONENT_DIR=${custom_secure_service_dir}
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
