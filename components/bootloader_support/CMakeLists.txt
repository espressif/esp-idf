set(COMPONENT_SRCS "src/bootloader_clock.c"
                   "src/bootloader_common.c"
                   "src/bootloader_flash.c"
                   "src/bootloader_random.c"
                   "src/bootloader_sha.c"
                   "src/bootloader_utility.c"
                   "src/esp_image_format.c"
                   "src/flash_encrypt.c"
                   "src/flash_partitions.c"
                   "src/flash_qio_mode.c"
                   "src/secure_boot.c"
                   "src/secure_boot_signatures.c")

if(${BOOTLOADER_BUILD})
    set(COMPONENT_ADD_INCLUDEDIRS "include include_bootloader")
    set(COMPONENT_REQUIRES)
    set(COMPONENT_PRIV_REQUIRES spi_flash micro-ecc efuse)
    list(APPEND COMPONENT_SRCS "src/bootloader_init.c")

    if(CONFIG_SECURE_SIGNED_APPS)
        get_filename_component(secure_boot_verification_key
            "signature_verification_key.bin"
            ABSOLUTE BASE_DIR "${CMAKE_BINARY_DIR}")
        if(CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES)
            add_custom_command(OUTPUT "${secure_boot_verification_key}"
                COMMAND ${ESPSECUREPY}
                    extract_public_key --keyfile "${secure_boot_signing_key}"
                    "${secure_boot_verification_key}"
                DEPENDS gen_secure_boot_signing_key
                VERBATIM)
        else()
            get_filename_component(orig_secure_boot_verification_key
                "${CONFIG_SECURE_BOOT_VERIFICATION_KEY}"
                ABSOLUTE BASE_DIR "${main_project_path}")
            if(NOT EXISTS ${orig_secure_boot_verification_key})
                message(FATAL_ERROR
                    "Secure Boot Verification Public Key ${CONFIG_SECURE_BOOT_VERIFICATION_KEY} does not exist."
                    "\nThis can be extracted from the private signing key."
                    "\nSee docs/security/secure-boot.rst for details.")
            endif()

            add_custom_command(OUTPUT "${secure_boot_verification_key}"
                COMMAND ${CMAKE_COMMAND} -E copy "${orig_secure_boot_verification_key}"
                    "${secure_boot_verification_key}"
                DEPENDS "${orig_secure_boot_verification_key}"
                VERBATIM)
        endif()
        set(COMPONENT_EMBED_FILES "${secure_boot_verification_key}")
        set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES
            "${secure_boot_verification_key}")
    endif()
else()
    set(COMPONENT_ADD_INCLUDEDIRS "include")
    set(COMPONENT_PRIV_INCLUDEDIRS "include_bootloader")
    set(COMPONENT_REQUIRES)
    set(COMPONENT_PRIV_REQUIRES spi_flash mbedtls efuse)
endif()

register_component()
