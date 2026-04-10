# Get picolibc specs option with --gc-sections flag removed
function(get_picolibc_specs_path out_var)
    set(modified_specs_path "${CMAKE_CURRENT_BINARY_DIR}/specs/picolibc.specs")
    set(${out_var} "${modified_specs_path}" PARENT_SCOPE)

    # Return if modified specs file already exists to avoid regeneration
    if(EXISTS "${modified_specs_path}")
        return()
    endif()

    # Find the original picolibc.specs file in the toolchain directory
    execute_process(
        COMMAND ${CMAKE_C_COMPILER} --print-file-name=picolibc.specs
        OUTPUT_VARIABLE picolibc_specs_path
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )

    if(picolibc_specs_path AND EXISTS "${picolibc_specs_path}")
        # Read the original specs file
        file(READ "${picolibc_specs_path}" specs_content)

        # Remove --gc-sections flag from the content
        string(REGEX REPLACE "--gc-sections" "" specs_content "${specs_content}")

        # Write the modified specs file
        file(WRITE "${modified_specs_path}" "${specs_content}")
    else()
        # Fallback: use original specs if not found
        message(WARNING "picolibc.specs not found in toolchain, using default")
        set(${out_var} "picolibc.specs" PARENT_SCOPE)
    endif()
endfunction()

if(CONFIG_IDF_TOOLCHAIN_GCC)
    if(CONFIG_STDATOMIC_S32C1I_SPIRAM_WORKAROUND)
        idf_toolchain_add_flags(COMPILE_OPTIONS "-mdisable-hardware-atomics")
    else()
        idf_toolchain_remove_flags(COMPILE_OPTIONS "-mdisable-hardware-atomics")
    endif()

    if(CONFIG_LIBC_PICOLIBC)
        # TODO GCC-464:
        # The original picolibc.specs file cannot be used directly in the build
        # system because it contains the --gc-sections linker flag. This flag
        # causes CMake's compiler detection checks to incorrectly drop required
        # data during configuration.
        #
        # Workaround: Copy picolibc.specs to the working directory with removed
        # --gc-sections option.
        get_picolibc_specs_path(picolibc_specs_path)
        idf_toolchain_add_flags(COMPILE_OPTIONS "\"-specs=${picolibc_specs_path}\"")
    else()
        idf_toolchain_remove_flags(COMPILE_OPTIONS "-specs=.*picolibc.*")
    endif()

    if(CONFIG_LIBC_NEWLIB_NANO_FORMAT)
        idf_toolchain_add_flags(LINK_OPTIONS "--specs=nano.specs")
    else()
        idf_toolchain_remove_flags(LINK_OPTIONS "--specs=nano.specs")
    endif()

    idf_toolchain_rerun_abi_detection()
else() # TODO IDF-14338
    if(CONFIG_STDATOMIC_S32C1I_SPIRAM_WORKAROUND)
        idf_build_set_property(COMPILE_OPTIONS "-mdisable-hardware-atomics" APPEND)
    endif()
endif()
