cmake_minimum_required(VERSION 3.5)

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

set(COMPONENTS bootloader esptool_py partition_table soc bootloader_support log spi_flash micro-ecc main efuse)
set(BOOTLOADER_BUILD 1)
include("${IDF_PATH}/tools/cmake/project.cmake")
set(common_req log esp_rom esp_common xtensa)
if(LEGACY_INCLUDE_COMMON_HEADERS)
    list(APPEND common_req soc)
endif()
idf_build_set_property(__COMPONENT_REQUIRES_COMMON "${common_req}")
idf_build_set_property(__OUTPUT_SDKCONFIG 0)
project(bootloader)

idf_build_set_property(COMPILE_DEFINITIONS "-DBOOTLOADER_BUILD=1" APPEND)
idf_build_set_property(COMPILE_OPTIONS "-fno-stack-protector" APPEND)

string(REPLACE ";" " " espsecurepy "${ESPSECUREPY}")
string(REPLACE ";" " " espefusepy "${ESPEFUSEPY}")
set(esptoolpy_write_flash "${ESPTOOLPY_WRITE_FLASH_STR}")

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
        MAIN_DEPENDENCY gen_project_binary
        DEPENDS gen_secure_bootloader_key gen_project_binary
        VERBATIM)

    add_custom_target (gen_bootloader_digest_bin ALL DEPENDS "${bootloader_digest_bin}")
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
            "\t${espefusepy} burn_key secure_boot ${secure_bootloader_key}"
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
            "* Not recommended to re-use the same secure boot keyfile on multiple production devices."
        DEPENDS gen_secure_bootloader_key gen_bootloader_digest_bin
        VERBATIM)
endif()
