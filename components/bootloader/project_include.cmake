set(BOOTLOADER_OFFSET 0x1000)

# Do not generate flash file when building bootloader
if(BOOTLOADER_BUILD)
    return()
endif()

# Glue to build the bootloader subproject binary as an external
# cmake project under this one
#
#
idf_build_get_property(build_dir BUILD_DIR)
set(BOOTLOADER_BUILD_DIR "${build_dir}/bootloader")
set(bootloader_binary_files
    "${BOOTLOADER_BUILD_DIR}/bootloader.elf"
    "${BOOTLOADER_BUILD_DIR}/bootloader.bin"
    "${BOOTLOADER_BUILD_DIR}/bootloader.map"
    )

idf_build_get_property(project_dir PROJECT_DIR)

# There are some additional processing when CONFIG_CONFIG_SECURE_SIGNED_APPS. This happens
# when either CONFIG_SECURE_BOOT_ENABLED or SECURE_BOOT_BUILD_SIGNED_BINARIES.
# For both cases, the user either sets binaries to be signed during build or not
# using CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES.
#
# Regardless, pass the main project's keys (signing/verification) to the bootloader subproject
# via config.
if(CONFIG_SECURE_SIGNED_APPS)
    add_custom_target(gen_secure_boot_keys)

    if(CONFIG_SECURE_BOOT_ENABLED)
        # Check that the configuration is sane
        if((CONFIG_SECURE_BOOTLOADER_REFLASHABLE AND CONFIG_SECURE_BOOTLOADER_ONE_TIME_FLASH) OR
            (NOT CONFIG_SECURE_BOOTLOADER_REFLASHABLE AND NOT CONFIG_SECURE_BOOTLOADER_ONE_TIME_FLASH))
            fail_at_build_time(bootloader "Invalid bootloader target: bad sdkconfig?")
        endif()

        if(CONFIG_SECURE_BOOTLOADER_REFLASHABLE)
            set(bootloader_binary_files
                ${bootloader_binary_files}
                "${BOOTLOADER_BUILD_DIR}/bootloader-reflash-digest.bin"
                "${BOOTLOADER_BUILD_DIR}/secure-bootloader-key-192.bin"
                "${BOOTLOADER_BUILD_DIR}/secure-bootloader-key-256.bin"
                )
        endif()
    endif()

    # Since keys are usually given relative to main project dir, get the absolute paths to the keys
    # for use by the bootloader subproject. Replace the values in config with these absolute paths,
    # so that bootloader subproject does not need to assume main project dir to obtain path to the keys.
    if(CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES)
        get_filename_component(secure_boot_signing_key
            "${CONFIG_SECURE_BOOT_SIGNING_KEY}"
            ABSOLUTE BASE_DIR "${project_dir}")

        if(NOT EXISTS ${secure_boot_signing_key})
            # If the signing key is not found, create a phony gen_secure_boot_signing_key target that
            # fails the build. fail_at_build_time causes a cmake run next time
            # (to pick up a new signing key if one exists, etc.)
            fail_at_build_time(gen_secure_boot_signing_key
                "Secure Boot Signing Key ${CONFIG_SECURE_BOOT_SIGNING_KEY} does not exist. Generate using:"
                "\tespsecure.py generate_signing_key ${CONFIG_SECURE_BOOT_SIGNING_KEY}")
        else()
            add_custom_target(gen_secure_boot_signing_key)
        endif()

        set(SECURE_BOOT_SIGNING_KEY ${secure_boot_signing_key}) # needed by some other components
        set(sign_key_arg "-DSECURE_BOOT_SIGNING_KEY=${secure_boot_signing_key}")

        add_dependencies(gen_secure_boot_keys gen_secure_boot_signing_key)
    else()

        get_filename_component(secure_boot_verification_key
            ${CONFIG_SECURE_BOOT_VERIFICATION_KEY}
            ABSOLUTE BASE_DIR "${project_dir}")

        if(NOT EXISTS ${secure_boot_verification_key})
            # If the verification key is not found, create a phony gen_secure_boot_verification_key target that
            # fails the build. fail_at_build_time causes a cmake run next time
            # (to pick up a new verification key if one exists, etc.)
            fail_at_build_time(gen_secure_boot_verification_key
                "Secure Boot Verification Public Key ${CONFIG_SECURE_BOOT_VERIFICATION_KEY} does not exist."
                "\tThis can be extracted from the private signing key."
                "\tSee docs/security/secure-boot.rst for details.")
        else()
            add_custom_target(gen_secure_boot_verification_key)
        endif()

        set(ver_key_arg "-DSECURE_BOOT_VERIFICATION_KEY=${secure_boot_verification_key}")

        add_dependencies(gen_secure_boot_keys gen_secure_boot_verification_key)
    endif()
endif()

idf_build_get_property(idf_path IDF_PATH)
idf_build_get_property(idf_target IDF_TARGET)
idf_build_get_property(sdkconfig SDKCONFIG)
idf_build_get_property(python PYTHON)

externalproject_add(bootloader
    SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/subproject"
    BINARY_DIR "${BOOTLOADER_BUILD_DIR}"
    CMAKE_ARGS  -DSDKCONFIG=${sdkconfig} -DIDF_PATH=${idf_path} -DIDF_TARGET=${idf_target}
                -DPYTHON_DEPS_CHECKED=1 -DPYTHON=${python}
                -DEXTRA_COMPONENT_DIRS=${CMAKE_CURRENT_LIST_DIR}
                ${sign_key_arg} ${ver_key_arg}
                # LEGACY_INCLUDE_COMMON_HEADERS has to be passed in via cache variable since
                # the bootloader common component requirements depends on this and
                # config variables are not available before project() call.
                -DLEGACY_INCLUDE_COMMON_HEADERS=${CONFIG_LEGACY_INCLUDE_COMMON_HEADERS}
    INSTALL_COMMAND ""
    BUILD_ALWAYS 1  # no easy way around this...
    BUILD_BYPRODUCTS ${bootloader_binary_files}
    )

if(CONFIG_SECURE_SIGNED_APPS)
    add_dependencies(bootloader gen_secure_boot_keys)
endif()

# this is a hack due to an (annoying) shortcoming in cmake, it can't
# extend the 'clean' target to the external project
# see thread: https://cmake.org/pipermail/cmake/2016-December/064660.html
#
# So for now we just have the top-level build remove the final build products...
set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" APPEND PROPERTY
    ADDITIONAL_MAKE_CLEAN_FILES
    ${bootloader_binary_files})