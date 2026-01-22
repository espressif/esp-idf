cmake_minimum_required(VERSION 3.22)

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
    esp_hal_security
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
    esp_libc
    esp_tee
    esp_stdio)

# EXTRA_COMPONENT_DIRS can be populated with directories containing one or several components.
# Make sure this variable contains `bootloader_components` directory of the project being compiled.
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
set(common_req log esp_rom esp_common esp_hw_support esp_libc)
idf_build_set_property(EXTRA_COMPONENT_EXCLUDE_DIRS "${EXTRA_COMPONENT_EXCLUDE_DIRS}")
idf_build_set_property(__COMPONENT_REQUIRES_COMMON "${common_req}")
idf_build_set_property(__OUTPUT_SDKCONFIG 0)
# Define a property for the default linker script
set(LD_DEFAULT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/main/ld/${IDF_TARGET}")
project(bootloader)
if(CONFIG_IDF_TARGET_ESP32P4 AND NOT CONFIG_ESP32P4_SELECTS_REV_LESS_V3)
    target_linker_script("__idf_main" INTERFACE "${LD_DEFAULT_PATH}/bootloader.rev3.ld.in")
else()
    target_linker_script("__idf_main" INTERFACE "${LD_DEFAULT_PATH}/bootloader.ld.in")
endif()

idf_build_set_property(COMPILE_DEFINITIONS "BOOTLOADER_BUILD=1" APPEND)
idf_build_set_property(COMPILE_DEFINITIONS "NON_OS_BUILD=1" APPEND)
idf_build_set_property(COMPILE_OPTIONS "-fno-stack-protector" APPEND)

# Set up the bootloader binary generation targets
set(PROJECT_BIN "bootloader.bin")
if(CONFIG_SECURE_BOOT_V2_ENABLED AND CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES)
    set(bootloader_unsigned_bin "bootloader-unsigned.bin")
else()
    set(bootloader_unsigned_bin "${PROJECT_BIN}")
endif()

# Set the final binary name as a project property
idf_build_set_property(PROJECT_BIN "${PROJECT_BIN}")

# Generate the unsigned binary from the ELF file.
if(CONFIG_APP_BUILD_GENERATE_BINARIES)
    set(binary_target_name "gen_bootloader_binary")
    __idf_build_binary("${bootloader_unsigned_bin}" "${binary_target_name}")
else()
    # If we are not building binaries, we don't need to create targets that depend on the
    # bootloader binary.
    return()
endif()

idf_component_get_property(main_args esptool_py FLASH_ARGS)
idf_component_get_property(sub_args esptool_py FLASH_SUB_ARGS)
idf_component_get_property(esptool_py_cmd esptool_py ESPTOOLPY_CMD)
idf_component_get_property(espsecure_py_cmd esptool_py ESPSECUREPY_CMD)
idf_component_get_property(espefuse_py_cmd esptool_py ESPEFUSEPY_CMD)

# String for printing flash command
string(REPLACE ";" " " esptoolpy_write_flash
    "${esptool_py_cmd} --port=(PORT) --baud=(BAUD) ${main_args} "
    "write-flash ${sub_args}")

string(REPLACE ";" " " espsecurepy "${espsecure_py_cmd}")
string(REPLACE ";" " " espefusepy "${espefuse_py_cmd}")

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
        COMMAND ${espsecure_py_cmd} digest-private-key
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
                "\n\t${espsecurepy} generate-flash-encryption-key ${secure_bootloader_key}"
                "\nIf a signing key is present, then instead use:"
                "\n\t${espsecurepy} digest-private-key "
                "--keylen (192/256) --keyfile KEYFILE "
                "${secure_bootloader_key}")
        endif()
        add_custom_target(gen_secure_bootloader_key)
    endif()

    add_custom_command(OUTPUT "${bootloader_digest_bin}"
        COMMAND ${CMAKE_COMMAND} -E echo "DIGEST ${bootloader_digest_bin}"
        COMMAND ${espsecure_py_cmd} digest-secure-bootloader --keyfile "${secure_bootloader_key}"
        -o "${bootloader_digest_bin}" "${CMAKE_BINARY_DIR}/bootloader.bin"
        MAIN_DEPENDENCY "${CMAKE_BINARY_DIR}/.bin_timestamp"
        DEPENDS gen_secure_bootloader_key gen_project_binary
        VERBATIM)

    add_custom_target(gen_bootloader_digest_bin ALL DEPENDS "${bootloader_digest_bin}")
endif()

# If secure boot is enabled, generate the signed binary from the unsigned one.
if(CONFIG_SECURE_BOOT_V2_ENABLED)
    set(signed_target_name "gen_signed_bootloader")

    if(CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES)
        # The SECURE_BOOT_SIGNING_KEY is passed in from the parent build and
        # is already an absolute path.
        if(NOT EXISTS "${SECURE_BOOT_SIGNING_KEY}")
            message(FATAL_ERROR
                "Secure Boot Signing Key Not found."
                "\nGenerate the Secure Boot V2 RSA-PSS 3072 Key."
                "\nTo generate one, you can use this command:"
                "\n\t${espsecurepy} generate-signing-key --version 2 your_key.pem"
            )
        endif()

        set(comment "Generated the signed Bootloader")
        set(key_arg KEYFILE "${SECURE_BOOT_SIGNING_KEY}")
        # Post-build commands should be attached to the signed binary target.
        set(post_build_target ${signed_target_name})
    else()
        # If we are not building signed binaries, we don't pass a key.
        set(comment "Bootloader generated but not signed")
        set(key_arg "")
        # Post-build commands should be attached to the unsigned binary target.
        set(post_build_target ${binary_target_name})
    endif()

    __idf_build_secure_binary("${bootloader_unsigned_bin}" "${PROJECT_BIN}" "${signed_target_name}"
        COMMENT "${comment}"
        ${key_arg}
    )
endif()

if(CONFIG_SECURE_BOOTLOADER_ONE_TIME_FLASH)
    add_custom_command(TARGET gen_project_binary POST_BUILD
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
    add_custom_command(TARGET gen_bootloader_digest_bin POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo
            "=============================================================================="
        COMMAND ${CMAKE_COMMAND} -E echo
            "Bootloader built and secure digest generated."
        COMMAND ${CMAKE_COMMAND} -E echo
            "Secure boot enabled, so bootloader not flashed automatically."
        COMMAND ${CMAKE_COMMAND} -E echo
            "Burn secure boot key to efuse using:"
        COMMAND ${CMAKE_COMMAND} -E echo
            "\t${espefusepy} burn-key secure_boot_v1 ${secure_bootloader_key}"
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
        VERBATIM)
elseif(
        CONFIG_SECURE_BOOT_V2_ENABLED AND
        (CONFIG_SOC_EFUSE_SECURE_BOOT_KEY_DIGESTS GREATER 1) AND
        NOT CONFIG_SECURE_BOOT_FLASH_BOOTLOADER_DEFAULT
    )
    add_custom_command(TARGET ${post_build_target} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo
        "=============================================================================="
    COMMAND ${CMAKE_COMMAND} -E echo
        "Bootloader built. Secure boot enabled, so bootloader not flashed automatically."
    COMMAND ${CMAKE_COMMAND} -E echo
        "To sign the bootloader with additional private keys."
    COMMAND ${CMAKE_COMMAND} -E echo
        "\t${espsecurepy} sign-data -k secure_boot_signing_key2.pem -v 2 \
--append-signatures -o signed_bootloader.bin build/bootloader/bootloader.bin"
    COMMAND ${CMAKE_COMMAND} -E echo
        "Secure boot enabled, so bootloader not flashed automatically."
    COMMAND ${CMAKE_COMMAND} -E echo
        "\t${esptoolpy_write_flash} ${BOOTLOADER_OFFSET} ${CMAKE_BINARY_DIR}/bootloader.bin"
    COMMAND ${CMAKE_COMMAND} -E echo
        "=============================================================================="
    VERBATIM)
elseif(CONFIG_SECURE_BOOT_V2_ENABLED AND NOT CONFIG_SECURE_BOOT_FLASH_BOOTLOADER_DEFAULT)
    add_custom_command(TARGET ${post_build_target} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo
        "=============================================================================="
    COMMAND ${CMAKE_COMMAND} -E echo
        "Bootloader built. Secure boot enabled, so bootloader not flashed automatically."
    COMMAND ${CMAKE_COMMAND} -E echo
        "\t${esptoolpy_write_flash} ${BOOTLOADER_OFFSET} ${CMAKE_BINARY_DIR}/bootloader.bin"
    COMMAND ${CMAKE_COMMAND} -E echo
        "=============================================================================="
    VERBATIM)
endif()

# Generate bootloader post-build check of the bootloader size against the offset
partition_table_add_check_bootloader_size_target(bootloader_check_size
    DEPENDS gen_project_binary
    BOOTLOADER_BINARY_PATH "${CMAKE_BINARY_DIR}/${PROJECT_BIN}"
    RESULT bootloader_check_size_command)
add_dependencies(app bootloader_check_size)

if(CONFIG_SECURE_BOOT_V2_ENABLED AND CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES)
    # Check the size of the bootloader + signature block.
    partition_table_add_check_bootloader_size_target(bootloader_check_size_signed
        DEPENDS gen_signed_bootloader
        BOOTLOADER_BINARY_PATH "${CMAKE_BINARY_DIR}/${PROJECT_BIN}"
        RESULT bootloader_check_size_signed_command)
    add_dependencies(app bootloader_check_size_signed)
endif()
