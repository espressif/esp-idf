set(BOOTLOADER_OFFSET ${CONFIG_BOOTLOADER_OFFSET_IN_FLASH})

# Do not generate flash file when building bootloader
if(BOOTLOADER_BUILD OR NOT CONFIG_APP_BUILD_BOOTLOADER)
    return()
endif()

# Glue to build the bootloader subproject binary as an external
# cmake project under this one
#
#
idf_build_get_property(build_dir BUILD_DIR)
set(BOOTLOADER_BUILD_DIR "${build_dir}/bootloader")
set(BOOTLOADER_ELF_FILE "${BOOTLOADER_BUILD_DIR}/bootloader.elf")
set(bootloader_binary_files
    "${BOOTLOADER_ELF_FILE}"
    "${BOOTLOADER_BUILD_DIR}/bootloader.bin"
    "${BOOTLOADER_BUILD_DIR}/bootloader.map"
    )

idf_build_get_property(project_dir PROJECT_DIR)

# There are some additional processing when CONFIG_SECURE_SIGNED_APPS. This happens
# when either CONFIG_SECURE_BOOT_V1_ENABLED or CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES.
# For both cases, the user either sets binaries to be signed during build or not
# using CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES.
#
# Regardless, pass the main project's keys (signing/verification) to the bootloader subproject
# via config.
if(CONFIG_SECURE_SIGNED_APPS)
    add_custom_target(gen_secure_boot_keys)

    if(CONFIG_SECURE_SIGNED_APPS_ECDSA_SCHEME)
        set(secure_apps_signing_version "1")
    elseif(CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME OR CONFIG_SECURE_SIGNED_APPS_ECDSA_V2_SCHEME)
        set(secure_apps_signing_version "2")
    endif()

    if(CONFIG_SECURE_BOOT_V1_ENABLED)
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
            if(CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME)
                fail_at_build_time(gen_secure_boot_signing_key
                    "Secure Boot Signing Key ${CONFIG_SECURE_BOOT_SIGNING_KEY} does not exist. Generate using:"
                    "\tespsecure.py generate_signing_key --version ${secure_apps_signing_version} \
                    ${CONFIG_SECURE_BOOT_SIGNING_KEY}")
            else()
                if(CONFIG_SECURE_BOOT_ECDSA_KEY_LEN_192_BITS)
                    set(scheme "ecdsa192")
                elseif(CONFIG_SECURE_BOOT_ECDSA_KEY_LEN_256_BITS)
                    set(scheme "ecdsa256")
                endif()
                fail_at_build_time(gen_secure_boot_signing_key
                    "Secure Boot Signing Key ${CONFIG_SECURE_BOOT_SIGNING_KEY} does not exist. Generate using:"
                    "\tespsecure.py generate_signing_key --version ${secure_apps_signing_version} \
                    --scheme ${scheme} ${CONFIG_SECURE_BOOT_SIGNING_KEY}")
            endif()
        else()
            add_custom_target(gen_secure_boot_signing_key)
        endif()

        set(SECURE_BOOT_SIGNING_KEY ${secure_boot_signing_key}) # needed by some other components
        set(sign_key_arg "-DSECURE_BOOT_SIGNING_KEY=${secure_boot_signing_key}")
        set(ver_key_arg)

        add_dependencies(gen_secure_boot_keys gen_secure_boot_signing_key)
    elseif(CONFIG_SECURE_SIGNED_APPS_ECDSA_SCHEME)

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
                "\tSee docs/security/secure-boot-v1.rst for details.")
        else()
            add_custom_target(gen_secure_boot_verification_key)
        endif()

        set(sign_key_arg)
        set(ver_key_arg "-DSECURE_BOOT_VERIFICATION_KEY=${secure_boot_verification_key}")

        add_dependencies(gen_secure_boot_keys gen_secure_boot_verification_key)
    endif()
else()
    set(sign_key_arg)
    set(ver_key_arg)
endif()

idf_build_get_property(idf_path IDF_PATH)
idf_build_get_property(idf_target IDF_TARGET)
idf_build_get_property(sdkconfig SDKCONFIG)
idf_build_get_property(python PYTHON)
idf_build_get_property(extra_cmake_args EXTRA_CMAKE_ARGS)

# We cannot pass lists are a parameter to the external project without modifying the ';' spearator
string(REPLACE ";" "|" BOOTLOADER_IGNORE_EXTRA_COMPONENT "${BOOTLOADER_IGNORE_EXTRA_COMPONENT}")

externalproject_add(bootloader
    SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/subproject"
    BINARY_DIR "${BOOTLOADER_BUILD_DIR}"
    # Modiying the list separator for the arguments, as such, we won't need to manually
    # replace the new separator by the default ';' in the subproject
    LIST_SEPARATOR |
    CMAKE_ARGS  -DSDKCONFIG=${sdkconfig} -DIDF_PATH=${idf_path} -DIDF_TARGET=${idf_target}
                -DPYTHON_DEPS_CHECKED=1 -DPYTHON=${python}
                -DEXTRA_COMPONENT_DIRS=${CMAKE_CURRENT_LIST_DIR}
                -DPROJECT_SOURCE_DIR=${PROJECT_SOURCE_DIR}
                -DIGNORE_EXTRA_COMPONENT=${BOOTLOADER_IGNORE_EXTRA_COMPONENT}
                ${sign_key_arg} ${ver_key_arg}
                ${extra_cmake_args}
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
    ADDITIONAL_CLEAN_FILES
    ${bootloader_binary_files})
