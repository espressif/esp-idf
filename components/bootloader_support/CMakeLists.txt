idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    return() # This component is not supported by the POSIX/Linux simulator
endif()

set(srcs
    "src/bootloader_common.c"
    "src/bootloader_common_loader.c"
    "src/bootloader_clock_init.c"
    "src/bootloader_mem.c"
    "src/bootloader_random.c"
    "src/bootloader_efuse.c"
    "src/flash_encrypt.c"
    "src/secure_boot.c"
    )

if(NOT CONFIG_IDF_ENV_FPGA)
    # For FPGA ENV, bootloader_random implementation is implemented in `bootloader_random.c`
    list(APPEND srcs "src/bootloader_random_${IDF_TARGET}.c")
endif()

if(NOT CONFIG_APP_BUILD_TYPE_PURE_RAM_APP)
    list(APPEND srcs
        "bootloader_flash/src/bootloader_flash.c"
        "bootloader_flash/src/flash_qio_mode.c"
        "bootloader_flash/src/bootloader_flash_config_${IDF_TARGET}.c"
        )
endif()

if(CONFIG_APP_BUILD_TYPE_APP_2NDBOOT)
    list(APPEND srcs
        "src/bootloader_utility.c"
        "src/flash_partitions.c"
        "src/esp_image_format.c"
        )
endif()

if(BOOTLOADER_BUILD OR CONFIG_APP_BUILD_TYPE_RAM)
    set(include_dirs "include" "bootloader_flash/include"
        "private_include")
    set(priv_requires micro-ecc spi_flash efuse esp_bootloader_format esp_app_format)
    list(APPEND srcs
    "src/bootloader_init.c"
    "src/bootloader_clock_loader.c"
    "src/bootloader_console.c"
    "src/bootloader_console_loader.c"
    "src/${IDF_TARGET}/bootloader_sha.c"
    "src/${IDF_TARGET}/bootloader_soc.c"
    "src/${IDF_TARGET}/bootloader_${IDF_TARGET}.c"
    )
    list(APPEND priv_requires hal)
    if(CONFIG_ESP_ROM_REV0_HAS_NO_ECDSA_INTERFACE)
        list(APPEND srcs
            "src/${IDF_TARGET}/bootloader_ecdsa.c")
    endif()
else()
    list(APPEND srcs
        "src/idf/bootloader_sha.c")
    set(include_dirs "include" "bootloader_flash/include")
    set(priv_include_dirs "private_include")
    # heap is required for `heap_memory_layout.h` header
    set(priv_requires spi_flash mbedtls efuse heap esp_bootloader_format esp_app_format)
endif()

if(BOOTLOADER_BUILD)
    list(APPEND srcs "src/bootloader_panic.c")
    if(CONFIG_SECURE_FLASH_ENC_ENABLED)
        list(APPEND srcs "src/flash_encryption/flash_encrypt.c"
                         "src/${IDF_TARGET}/flash_encryption_secure_features.c")
    endif()

    if(CONFIG_SECURE_SIGNED_ON_BOOT)
        if(CONFIG_SECURE_SIGNED_APPS_ECDSA_SCHEME)
            list(APPEND srcs "src/secure_boot_v1/secure_boot_signatures_bootloader.c")
        endif()
        if(CONFIG_SECURE_BOOT_V1_ENABLED)
            list(APPEND srcs "src/secure_boot_v1/secure_boot.c"
                             "src/${IDF_TARGET}/secure_boot_secure_features.c")
        endif()

        if(CONFIG_SECURE_BOOT_V2_ENABLED)
            list(APPEND srcs "src/secure_boot_v2/secure_boot_signatures_bootloader.c"
                             "src/secure_boot_v2/secure_boot.c"
                             "src/${IDF_TARGET}/secure_boot_secure_features.c")
        endif()
    endif()
else()
    if(CONFIG_SECURE_SIGNED_ON_UPDATE)
        if(CONFIG_SECURE_SIGNED_APPS_ECDSA_SCHEME)
            list(APPEND srcs "src/secure_boot_v1/secure_boot_signatures_app.c")
        endif()

        if(CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME)
            list(APPEND srcs "src/secure_boot_v2/secure_boot_signatures_app.c")
            list(APPEND srcs "src/secure_boot_v2/secure_boot_rsa_signature.c")
        endif()
        if(CONFIG_SECURE_SIGNED_APPS_ECDSA_V2_SCHEME)
            list(APPEND srcs "src/secure_boot_v2/secure_boot_signatures_app.c")
            list(APPEND srcs "src/secure_boot_v2/secure_boot_ecdsa_signature.c")
        endif()
    endif()
endif()

set(requires soc) #unfortunately the header directly uses SOC registers

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "${include_dirs}"
                    PRIV_INCLUDE_DIRS "${priv_include_dirs}"
                    REQUIRES "${requires}"
                    PRIV_REQUIRES "${priv_requires}")

if(NOT BOOTLOADER_BUILD)
    if(CONFIG_SECURE_SIGNED_ON_UPDATE)
        if(CONFIG_SECURE_SIGNED_APPS_ECDSA_SCHEME OR CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME OR
           CONFIG_SECURE_SIGNED_APPS_ECDSA_V2_SCHEME)
            target_link_libraries(${COMPONENT_LIB} PRIVATE idf::app_update)
        endif()
    endif()
endif()

if(CONFIG_SECURE_SIGNED_APPS AND (CONFIG_SECURE_BOOT_V1_ENABLED OR CONFIG_SECURE_SIGNED_APPS_ECDSA_SCHEME))
    if(BOOTLOADER_BUILD)
        # Whether CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES or not, we need verification key to embed
        # in the library.
        if(CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES)
            # We generate the key from the signing key. The signing key is passed from the main project.
            get_filename_component(secure_boot_signing_key
                "${SECURE_BOOT_SIGNING_KEY}"
                ABSOLUTE BASE_DIR "${project_dir}")
            get_filename_component(secure_boot_verification_key
                "signature_verification_key.bin"
                ABSOLUTE BASE_DIR "${CMAKE_CURRENT_BINARY_DIR}")
            add_custom_command(OUTPUT "${secure_boot_verification_key}"
                COMMAND ${ESPSECUREPY}
                extract_public_key --keyfile "${secure_boot_signing_key}"
                "${secure_boot_verification_key}"
                DEPENDS ${secure_boot_signing_key}
                VERBATIM)
        else()
            # We expect to 'inherit' the verification key passed from main project.
            get_filename_component(secure_boot_verification_key
                ${SECURE_BOOT_VERIFICATION_KEY}
                ABSOLUTE BASE_DIR "${project_dir}")
        endif()
    else()  # normal app build
        idf_build_get_property(project_dir PROJECT_DIR)

        if(CONFIG_SECURE_BOOT_VERIFICATION_KEY)
            # verification-only build supplies verification key
            set(secure_boot_verification_key ${CONFIG_SECURE_BOOT_VERIFICATION_KEY})
            get_filename_component(secure_boot_verification_key
                ${secure_boot_verification_key}
                ABSOLUTE BASE_DIR "${project_dir}")
        else()
            # sign at build time, extracts key from signing key
            set(secure_boot_verification_key "${CMAKE_BINARY_DIR}/signature_verification_key.bin")
            get_filename_component(secure_boot_signing_key
                ${CONFIG_SECURE_BOOT_SIGNING_KEY}
                ABSOLUTE BASE_DIR "${project_dir}")

            add_custom_command(OUTPUT "${secure_boot_verification_key}"
                COMMAND ${ESPSECUREPY}
                extract_public_key --keyfile "${secure_boot_signing_key}"
                "${secure_boot_verification_key}"
                WORKING_DIRECTORY ${project_dir}
                DEPENDS ${secure_boot_signing_key}
                VERBATIM)
        endif()
    endif()

    # Embed the verification key in the binary (app & bootloader)
    #
    target_add_binary_data(${COMPONENT_LIB} "${secure_boot_verification_key}" "BINARY"
        RENAME_TO signature_verification_key_bin)
    set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        APPEND PROPERTY ADDITIONAL_CLEAN_FILES
        "${secure_boot_verification_key}")
endif()

if(BOOTLOADER_BUILD)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u abort")
    # esp_bootloader_desc structure is added as an undefined symbol because otherwise the
    # linker will ignore this structure as it has no other files depending on it.
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u esp_bootloader_desc")
endif()
