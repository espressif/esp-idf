cmake_minimum_required(VERSION 3.22)

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
list(APPEND COMPONENTS bootloader_support efuse esp_hal_mspi esp_hal_wdt esp_security mbedtls esp_stdio)

# TEE-specific components
list(APPEND COMPONENTS attestation tee_flash_mgr tee_ota_ops tee_sec_storage)

include_directories("${SECURE_SERVICE_HEADERS_DIR}")

include("${IDF_PATH}/tools/cmake/project.cmake")
set(common_req esp_common esp_hw_support esp_rom freertos hal log esp_libc soc spi_flash)

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

# Set up the TEE binary generation targets
set(project_bin "esp_tee.bin")
if(CONFIG_SECURE_BOOT_V2_ENABLED AND CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES)
    set(esp_tee_unsigned_bin "esp_tee-unsigned.bin")
else()
    set(esp_tee_unsigned_bin "${project_bin}")
endif()

# Set the final binary name as a project property.
idf_build_set_property(PROJECT_BIN "${project_bin}")

# Generate the unsigned binary from the ELF file.
if(CONFIG_APP_BUILD_GENERATE_BINARIES)
    set(target_name "gen_esp_tee_binary")
    __idf_build_binary("${esp_tee_unsigned_bin}" "${target_name}")
endif()

idf_component_get_property(espsecure_py_cmd esptool_py ESPSECUREPY_CMD)

# If secure boot is enabled, generate the signed binary from the unsigned one.
if(CONFIG_SECURE_BOOT_V2_ENABLED)
    set(target_name "gen_signed_esp_tee_binary")

    if(CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES)
        # The SECURE_BOOT_SIGNING_KEY is passed in from the parent build and
        # is already an absolute path.
        if(NOT EXISTS "${SECURE_BOOT_SIGNING_KEY}")
            message(FATAL_ERROR
                "Secure Boot Signing Key Not found."
                "\nGenerate the Secure Boot V2 RSA-PSS 3072 Key."
                "\nTo generate one, you can use this command:"
                "\n\t${espsecure_py_cmd} generate-signing-key --version 2 your_key.pem"
            )
        endif()

        set(comment "Generated the signed TEE")
        set(key_arg KEYFILE "${SECURE_BOOT_SIGNING_KEY}")
    else()
        # If we are not building signed binaries, we don't pass a key.
        set(comment "TEE generated but not signed")
        set(key_arg "")
    endif()

    __idf_build_secure_binary("${esp_tee_unsigned_bin}" "${project_bin}" "${target_name}"
        COMMENT "${comment}"
        ${key_arg}
    )
endif()
