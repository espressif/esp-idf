cmake_minimum_required(VERSION 3.16)

include("${CMAKE_CURRENT_LIST_DIR}/../version.cmake")

if(NOT SDKCONFIG)
    message(FATAL_ERROR "esp_tee subproject expects the SDKCONFIG variable to be passed "
        "in by the parent build process.")
endif()

if(NOT IDF_PATH)
    message(FATAL_ERROR "esp_tee subproject expects the IDF_PATH variable to be passed "
        "in by the parent build process.")
endif()

if(NOT IDF_TARGET)
    message(FATAL_ERROR "esp_tee subproject expects the IDF_TARGET variable to be passed "
        "in by the parent build process.")
endif()

set(COMPONENTS esp_tee bootloader esptool_py partition_table main ${CUSTOM_SECURE_SERVICE_COMPONENT})
list(APPEND EXTRA_COMPONENT_DIRS ${CUSTOM_SECURE_SERVICE_COMPONENT_DIR})
set(ESP_TEE_BUILD 1)
set(NON_OS_BUILD 1)

# Additional components
list(APPEND COMPONENTS bootloader_support efuse esp_security mbedtls)

# TEE-specific components
list(APPEND COMPONENTS tee_flash_mgr tee_ota_ops tee_sec_storage tee_attestation)

include_directories("${SECURE_SERVICE_HEADERS_DIR}")

include("${IDF_PATH}/tools/cmake/project.cmake")
set(common_req esp_common esp_hw_support esp_rom freertos hal log newlib soc spi_flash)

if(CONFIG_IDF_TARGET_ARCH_RISCV)
    list(APPEND common_req riscv)
endif()

idf_build_set_property(__COMPONENT_REQUIRES_COMMON "${common_req}")
idf_build_set_property(__OUTPUT_SDKCONFIG 0)
# NOTE: Helps to analyse the components built for the TEE binary by CMake Graphviz
idf_build_set_property(__BUILD_COMPONENT_DEPGRAPH_ENABLED 1)

# NOTE: <TEE source version>~v<IDF release> - the same TEE source builds differently per IDF release
set(esp_tee_ver "${ESP_TEE_VERSION_MAJOR}.${ESP_TEE_VERSION_MINOR}.${ESP_TEE_VERSION_PATCH}")
set(PROJECT_VER "${esp_tee_ver}~v${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}.${IDF_VERSION_PATCH}")
project(esp_tee VERSION ${ESP_TEE_VERSION_MAJOR}.${ESP_TEE_VERSION_MINOR}.${ESP_TEE_VERSION_PATCH})

# NOTE: Forced by the parent build (see project_include.cmake); the TEE image must stay reproducible
if(NOT CONFIG_APP_REPRODUCIBLE_BUILD OR CONFIG_APP_PROJECT_VER_FROM_CONFIG)
    message(FATAL_ERROR "esp_tee: TEE build must be reproducible (see project_include.cmake)")
endif()

idf_build_set_property(COMPILE_DEFINITIONS "ESP_TEE_BUILD=1" APPEND)
idf_build_set_property(COMPILE_DEFINITIONS "NON_OS_BUILD=1" APPEND)
idf_build_set_property(COMPILE_OPTIONS "-fno-stack-protector" APPEND)
# NOTE: Any __DATE__/__TIME__/__TIMESTAMP__ use breaks reproducibility
idf_build_set_property(COMPILE_OPTIONS "-Werror=date-time" APPEND)

# NOTE: Custom secure-service components live outside IDF_PATH and would otherwise
# embed absolute __FILE__ paths, breaking TEE binary reproducibility
if(CUSTOM_SECURE_SERVICE_COMPONENT_DIR)
    idf_build_set_property(COMPILE_OPTIONS
        "-fmacro-prefix-map=${CUSTOM_SECURE_SERVICE_COMPONENT_DIR}=/CUSTOM_SEC_SRV" APPEND)
endif()

# NOTE: The generated secure-service headers live in the parent (REE) build directory;
# map them out of DWARF as the ELF digest is embedded in the TEE binary
if(SECURE_SERVICE_HEADERS_DIR)
    idf_build_set_property(COMPILE_OPTIONS
        "-fdebug-prefix-map=${SECURE_SERVICE_HEADERS_DIR}=/SEC_SRV_HDRS" APPEND)
endif()

if(CONFIG_SECURE_BOOT_V2_ENABLED)
    if(CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES)
        get_filename_component(secure_boot_signing_key
            "${SECURE_BOOT_SIGNING_KEY}" ABSOLUTE BASE_DIR "${project_dir}")

        if(NOT EXISTS "${secure_boot_signing_key}")
        message(FATAL_ERROR
            "Secure Boot Signing Key Not found."
            "\nGenerate the Secure Boot V2 RSA-PSS 3072 Key."
            "\nTo generate one, you can use this command:"
            "\n\t${espsecurepy} generate_signing_key --version 2 ${SECURE_BOOT_SIGNING_KEY}")
        endif()

        set(esp_tee_unsigned_bin "esp_tee-unsigned.bin")
        add_custom_command(OUTPUT ".signed_bin_timestamp"
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/${PROJECT_BIN}"
            "${CMAKE_BINARY_DIR}/${esp_tee_unsigned_bin}"
            COMMAND ${ESPSECUREPY} sign_data --version 2 --keyfile "${secure_boot_signing_key}"
            -o "${CMAKE_BINARY_DIR}/${PROJECT_BIN}" "${CMAKE_BINARY_DIR}/${esp_tee_unsigned_bin}"
            COMMAND ${CMAKE_COMMAND} -E echo "Generated signed binary image ${build_dir}/${PROJECT_BIN}"
            "from ${CMAKE_BINARY_DIR}/${esp_tee_unsigned_bin}"
            COMMAND ${CMAKE_COMMAND} -E md5sum "${CMAKE_BINARY_DIR}/${PROJECT_BIN}"
            > "${CMAKE_BINARY_DIR}/.signed_bin_timestamp"
            DEPENDS "${build_dir}/.bin_timestamp"
            VERBATIM
            COMMENT "Generated the signed TEE")
    else()
        add_custom_command(OUTPUT ".signed_bin_timestamp"
        VERBATIM
        COMMENT "TEE generated but not signed")
    endif()

    add_custom_target(gen_signed_esp_tee ALL DEPENDS "${build_dir}/.signed_bin_timestamp")
endif()
