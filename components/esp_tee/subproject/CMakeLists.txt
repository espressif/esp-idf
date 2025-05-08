cmake_minimum_required(VERSION 3.16)

set(ESP_TEE_VERSION_MAJOR 1)
set(ESP_TEE_VERSION_MINOR 0)
set(ESP_TEE_VERSION_PATCH 0)

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

# Include sdkconfig.h derived from the parent build.
include_directories(${CONFIG_DIR})

include("${IDF_PATH}/tools/cmake/project.cmake")
set(common_req esp_common esp_hw_support esp_rom freertos hal log newlib soc spi_flash)

if(CONFIG_IDF_TARGET_ARCH_RISCV)
    list(APPEND common_req riscv)
endif()

idf_build_set_property(__COMPONENT_REQUIRES_COMMON "${common_req}")
idf_build_set_property(__OUTPUT_SDKCONFIG 0)
# NOTE: Helps to analyse the components built for the TEE binary by CMake Graphviz
idf_build_set_property(__BUILD_COMPONENT_DEPGRAPH_ENABLED 1)

project(esp_tee VERSION ${ESP_TEE_VERSION_MAJOR}.${ESP_TEE_VERSION_MINOR}.${ESP_TEE_VERSION_PATCH})

idf_build_set_property(COMPILE_DEFINITIONS "ESP_TEE_BUILD=1" APPEND)
idf_build_set_property(COMPILE_DEFINITIONS "NON_OS_BUILD=1" APPEND)
idf_build_set_property(COMPILE_OPTIONS "-fno-stack-protector" APPEND)

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
