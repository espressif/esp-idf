set(srcs 
    "src/bootloader_clock.c"
    "src/bootloader_common.c"
    "src/bootloader_flash.c"
    "src/bootloader_flash_config.c"
    "src/bootloader_random.c"
    "src/bootloader_utility.c"
    "src/esp_image_format.c"
    "src/flash_encrypt.c"
    "src/flash_partitions.c"
    "src/flash_qio_mode.c")

if(BOOTLOADER_BUILD)
    set(include_dirs "include" "include_bootloader")
    set(requires soc)  #unfortunately the header directly uses SOC registers
    set(priv_requires micro-ecc spi_flash efuse)
    list(APPEND srcs 
        "src/bootloader_init.c"
        "src/${IDF_TARGET}/bootloader_sha.c"
        "src/${IDF_TARGET}/flash_encrypt.c"
        "src/${IDF_TARGET}/secure_boot_signatures.c"
        "src/${IDF_TARGET}/secure_boot.c")
else()
    list(APPEND srcs 
        "src/idf/bootloader_sha.c"
        "src/idf/secure_boot_signatures.c")
    set(include_dirs "include")
    set(priv_include_dirs "include_bootloader")
    set(requires soc) #unfortunately the header directly uses SOC registers
    set(priv_requires spi_flash mbedtls efuse)
endif()

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "${include_dirs}"
                    PRIV_INCLUDE_DIRS "${priv_include_dirs}"
                    REQUIRES "${requires}"
                    PRIV_REQUIRES "${priv_requires}")

if(BOOTLOADER_BUILD AND CONFIG_SECURE_SIGNED_APPS)
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
            VERBATIM)
    else()
        # We expect to 'inherit' the verification key passed from main project.
        get_filename_component(secure_boot_verification_key
            ${SECURE_BOOT_VERIFICATION_KEY}
            ABSOLUTE BASE_DIR "${project_dir}")
    endif()

    target_add_binary_data(${COMPONENT_LIB} "${secure_boot_verification_key}" "BINARY")
    set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
        APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES
        "${secure_boot_verification_key}")
endif()
