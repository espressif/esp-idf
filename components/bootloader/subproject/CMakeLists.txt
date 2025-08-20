cmake_minimum_required(VERSION 3.16)

if(NOT SDKCONFIG)
    message(FATAL_ERROR "Bootloader subproject expects the SDKCONFIG variable to be passed "
        "in by the parent build process.")
endif()

if(NOT IDF_PATH)
    message(FATAL_ERROR "Bootloader subproject expects the IDF_PATH variable to be passed "
        "in by the parent build process.")
endif()

if(NOT IDF_TARGET)
    message(FATAL_ERROR "Bootloader subproject expects the IDF_TARGET variable to be passed "
        "in by the parent build process.")
endif()

# A number of these components are implemented as config-only when built in the bootloader
set(COMPONENTS
    bootloader
    esptool_py
    esp_hw_support
    esp_system
    freertos
    hal
    partition_table
    soc
    bootloader_support
    log
    spi_flash
    micro-ecc
    main
    efuse
    esp_system
    newlib)

# Make EXTRA_COMPONENT_DIRS variable to point to the bootloader_components directory
# of the project being compiled
set(PROJECT_EXTRA_COMPONENTS "${PROJECT_SOURCE_DIR}/bootloader_components")
if(EXISTS ${PROJECT_EXTRA_COMPONENTS})
    list(APPEND EXTRA_COMPONENT_DIRS "${PROJECT_EXTRA_COMPONENTS}")
endif()

if(IGNORE_EXTRA_COMPONENT)
    # Prefix all entries of the list with ${PROJECT_EXTRA_COMPONENTS} absolute path
    list(TRANSFORM IGNORE_EXTRA_COMPONENT
         PREPEND "${PROJECT_EXTRA_COMPONENTS}/"
         OUTPUT_VARIABLE EXTRA_COMPONENT_EXCLUDE_DIRS)
endif()

# Consider each directory in the project's bootloader_components as a component to be compiled
file(GLOB proj_components RELATIVE ${PROJECT_EXTRA_COMPONENTS} ${PROJECT_EXTRA_COMPONENTS}/*)
foreach(component ${proj_components})
  # Only directories are considered components
  if(IS_DIRECTORY "${PROJECT_EXTRA_COMPONENTS}/${component}" AND NOT ${component} IN_LIST IGNORE_EXTRA_COMPONENT)
    list(APPEND COMPONENTS ${component})
  endif()
endforeach()

set(BOOTLOADER_BUILD 1)
set(NON_OS_BUILD 1)
include("${IDF_PATH}/tools/cmake/project.cmake")
set(common_req log esp_rom esp_common esp_hw_support newlib)
idf_build_set_property(EXTRA_COMPONENT_EXCLUDE_DIRS "${EXTRA_COMPONENT_EXCLUDE_DIRS}")
idf_build_set_property(__COMPONENT_REQUIRES_COMMON "${common_req}")
idf_build_set_property(__OUTPUT_SDKCONFIG 0)
project(bootloader)

idf_build_set_property(COMPILE_DEFINITIONS "BOOTLOADER_BUILD=1" APPEND)
idf_build_set_property(COMPILE_DEFINITIONS "NON_OS_BUILD=1" APPEND)
idf_build_set_property(COMPILE_OPTIONS "-fno-stack-protector" APPEND)

idf_component_get_property(main_args esptool_py FLASH_ARGS)
idf_component_get_property(sub_args esptool_py FLASH_SUB_ARGS)

# String for printing flash command
string(REPLACE ";" " " esptoolpy_write_flash
    "${ESPTOOLPY} --port=(PORT) --baud=(BAUD) ${main_args} "
    "write_flash ${sub_args}")

string(REPLACE ";" " " espsecurepy "${ESPSECUREPY}")
string(REPLACE ";" " " espefusepy "${ESPEFUSEPY}")

# Suppress warning: "Manually-specified variables were not used by the project: SECURE_BOOT_SIGNING_KEY"
set(ignore_signing_key "${SECURE_BOOT_SIGNING_KEY}")

if(CONFIG_SECURE_BOOTLOADER_REFLASHABLE)
    if(CONFIG_SECURE_BOOTLOADER_KEY_ENCODING_192BIT)
        set(key_digest_len 192)
    else()
        set(key_digest_len 256)
    endif()

    get_filename_component(bootloader_digest_bin
        "bootloader-reflash-digest.bin"
        ABSOLUTE BASE_DIR "${CMAKE_BINARY_DIR}")

    get_filename_component(secure_bootloader_key
        "secure-bootloader-key-${key_digest_len}.bin"
        ABSOLUTE BASE_DIR "${CMAKE_BINARY_DIR}")

    add_custom_command(OUTPUT "${secure_bootloader_key}"
        COMMAND ${ESPSECUREPY} digest_private_key
            --keylen "${key_digest_len}"
            --keyfile "${SECURE_BOOT_SIGNING_KEY}"
            "${secure_bootloader_key}"
        VERBATIM)

    if(CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES)
        add_custom_target(gen_secure_bootloader_key ALL DEPENDS "${secure_bootloader_key}")
    else()
        if(NOT EXISTS "${secure_bootloader_key}")
            message(FATAL_ERROR
                "No pre-generated key for a reflashable secure bootloader is available, "
                "due to signing configuration."
                "\nTo generate one, you can use this command:"
                "\n\t${espsecurepy} generate_flash_encryption_key ${secure_bootloader_key}"
                "\nIf a signing key is present, then instead use:"
                "\n\t${espsecurepy} digest_private_key "
                "--keylen (192/256) --keyfile KEYFILE "
                "${secure_bootloader_key}")
        endif()
        add_custom_target(gen_secure_bootloader_key)
    endif()

    add_custom_command(OUTPUT "${bootloader_digest_bin}"
        COMMAND ${CMAKE_COMMAND} -E echo "DIGEST ${bootloader_digest_bin}"
        COMMAND ${ESPSECUREPY} digest_secure_bootloader --keyfile "${secure_bootloader_key}"
        -o "${bootloader_digest_bin}" "${CMAKE_BINARY_DIR}/bootloader.bin"
        MAIN_DEPENDENCY "${CMAKE_BINARY_DIR}/.bin_timestamp"
        DEPENDS gen_secure_bootloader_key gen_project_binary
        VERBATIM)

    add_custom_target(gen_bootloader_digest_bin ALL DEPENDS "${bootloader_digest_bin}")
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

        set(bootloader_unsigned_bin "bootloader-unsigned.bin")
        add_custom_command(OUTPUT ".signed_bin_timestamp"
            COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/${PROJECT_BIN}"
            "${CMAKE_BINARY_DIR}/${bootloader_unsigned_bin}"
            COMMAND ${ESPSECUREPY} sign_data --version 2 --keyfile "${secure_boot_signing_key}"
            -o "${CMAKE_BINARY_DIR}/${PROJECT_BIN}" "${CMAKE_BINARY_DIR}/${bootloader_unsigned_bin}"
            COMMAND ${CMAKE_COMMAND} -E echo "Generated signed binary image ${build_dir}/${PROJECT_BIN}"
            "from ${CMAKE_BINARY_DIR}/${bootloader_unsigned_bin}"
            COMMAND ${CMAKE_COMMAND} -E md5sum "${CMAKE_BINARY_DIR}/${PROJECT_BIN}"
            > "${CMAKE_BINARY_DIR}/.signed_bin_timestamp"
            DEPENDS "${build_dir}/.bin_timestamp"
            VERBATIM
            COMMENT "Generated the signed Bootloader")
    else()
        add_custom_command(OUTPUT ".signed_bin_timestamp"
        VERBATIM
        COMMENT "Bootloader generated but not signed")
    endif()

    add_custom_target(gen_signed_bootloader ALL DEPENDS "${build_dir}/.signed_bin_timestamp")
endif()

if(CONFIG_SECURE_BOOTLOADER_ONE_TIME_FLASH)
    add_custom_command(TARGET bootloader.elf POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo
            "=============================================================================="
        COMMAND ${CMAKE_COMMAND} -E echo
            "Bootloader built. Secure boot enabled, so bootloader not flashed automatically."
        COMMAND ${CMAKE_COMMAND} -E echo
            "One-time flash command is:"
        COMMAND ${CMAKE_COMMAND} -E echo
            "\t${esptoolpy_write_flash} ${BOOTLOADER_OFFSET} ${CMAKE_BINARY_DIR}/bootloader.bin"
        COMMAND ${CMAKE_COMMAND} -E echo
            "* IMPORTANT: After first boot, BOOTLOADER CANNOT BE RE-FLASHED on same device"
        VERBATIM)
elseif(CONFIG_SECURE_BOOTLOADER_REFLASHABLE)
    add_custom_command(TARGET bootloader.elf POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo
            "=============================================================================="
        COMMAND ${CMAKE_COMMAND} -E echo
            "Bootloader built and secure digest generated."
        COMMAND ${CMAKE_COMMAND} -E echo
            "Secure boot enabled, so bootloader not flashed automatically."
        COMMAND ${CMAKE_COMMAND} -E echo
            "Burn secure boot key to efuse using:"
        COMMAND ${CMAKE_COMMAND} -E echo
            "\t${espefusepy} burn_key secure_boot_v1 ${secure_bootloader_key}"
        COMMAND ${CMAKE_COMMAND} -E echo
            "First time flash command is:"
        COMMAND ${CMAKE_COMMAND} -E echo
            "\t${esptoolpy_write_flash} ${BOOTLOADER_OFFSET} ${CMAKE_BINARY_DIR}/bootloader.bin"
        COMMAND ${CMAKE_COMMAND} -E echo
            "=============================================================================="
        COMMAND ${CMAKE_COMMAND} -E echo
            "To reflash the bootloader after initial flash:"
        COMMAND ${CMAKE_COMMAND} -E echo
            "\t${esptoolpy_write_flash} 0x0 ${bootloader_digest_bin}"
        COMMAND ${CMAKE_COMMAND} -E echo
            "=============================================================================="
        COMMAND ${CMAKE_COMMAND} -E echo
            "* After first boot, only re-flashes of this kind (with same key) will be accepted."
        COMMAND ${CMAKE_COMMAND} -E echo
            "* Not recommended to reuse the same secure boot keyfile on multiple production devices."
        DEPENDS gen_secure_bootloader_key gen_bootloader_digest_bin
        VERBATIM)
elseif(
        CONFIG_SECURE_BOOT_V2_ENABLED AND
        (CONFIG_SOC_EFUSE_SECURE_BOOT_KEY_DIGESTS GREATER 1) AND
        NOT CONFIG_SECURE_BOOT_FLASH_BOOTLOADER_DEFAULT
    )
    add_custom_command(TARGET bootloader.elf POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo
        "=============================================================================="
    COMMAND ${CMAKE_COMMAND} -E echo
        "Bootloader built. Secure boot enabled, so bootloader not flashed automatically."
    COMMAND ${CMAKE_COMMAND} -E echo
        "To sign the bootloader with additional private keys."
    COMMAND ${CMAKE_COMMAND} -E echo
        "\t${espsecurepy} sign_data -k secure_boot_signing_key2.pem -v 2 \
--append_signatures -o signed_bootloader.bin build/bootloader/bootloader.bin"
    COMMAND ${CMAKE_COMMAND} -E echo
        "Secure boot enabled, so bootloader not flashed automatically."
    COMMAND ${CMAKE_COMMAND} -E echo
        "\t${esptoolpy_write_flash} ${BOOTLOADER_OFFSET} ${CMAKE_BINARY_DIR}/bootloader.bin"
    COMMAND ${CMAKE_COMMAND} -E echo
        "=============================================================================="
    DEPENDS gen_signed_bootloader
    VERBATIM)
elseif(CONFIG_SECURE_BOOT_V2_ENABLED AND NOT CONFIG_SECURE_BOOT_FLASH_BOOTLOADER_DEFAULT)
    add_custom_command(TARGET bootloader.elf POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo
        "=============================================================================="
    COMMAND ${CMAKE_COMMAND} -E echo
        "Bootloader built. Secure boot enabled, so bootloader not flashed automatically."
    COMMAND ${CMAKE_COMMAND} -E echo
        "Secure boot enabled, so bootloader not flashed automatically."
    COMMAND ${CMAKE_COMMAND} -E echo
        "\t${esptoolpy_write_flash} ${BOOTLOADER_OFFSET} ${CMAKE_BINARY_DIR}/bootloader.bin"
    COMMAND ${CMAKE_COMMAND} -E echo
        "=============================================================================="
    DEPENDS gen_signed_bootloader
    VERBATIM)
endif()
