function(_get_modified_specs_path out_var specs_filename modification)
    set(modified_specs_path "${CMAKE_CURRENT_BINARY_DIR}/specs/${specs_filename}")
    set(${out_var} "${modified_specs_path}" PARENT_SCOPE)

    # Return if modified specs file already exists to avoid regeneration
    if(EXISTS "${modified_specs_path}")
        return()
    endif()

    # Find the original specs file in the toolchain directory
    execute_process(
        COMMAND ${CMAKE_C_COMPILER} --print-file-name=${specs_filename}
        OUTPUT_VARIABLE specs_path
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )

    if(specs_path AND EXISTS "${specs_path}")
        file(READ "${specs_path}" specs_content)

        if(modification STREQUAL "remove_gc_sections")
            string(REGEX REPLACE "--gc-sections" "" specs_content "${specs_content}")
        elseif(modification STREQUAL "add_nosys_to_lib")
            set(original_lib_specs [=[
%{!shared:%{g*:-lg_nano} %{!p:%{!pg:-lc_nano}}%{p:-lc_p}%{pg:-lc_p}}
]=])
            set(modified_lib_specs [=[
%{!shared:%{g*:-lg_nano} %{!p:%{!pg:-lc_nano -lnosys -lc_nano}}%{p:-lc_p}%{pg:-lc_p}}
]=])
            string(REPLACE "${original_lib_specs}" "${modified_lib_specs}" specs_content "${specs_content}")
        endif()

        file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/specs")
        file(WRITE "${modified_specs_path}" "${specs_content}")
    else()
        # Fallback: use original specs if not found
        message(WARNING "${specs_filename} not found in toolchain, using default")
        set(${out_var} "${specs_filename}" PARENT_SCOPE)
    endif()
endfunction()

# Get picolibc specs option with --gc-sections flag removed
function(get_picolibc_specs_path out_var)
    _get_modified_specs_path(specs_path picolibc.specs remove_gc_sections)
    set(${out_var} "${specs_path}" PARENT_SCOPE)
endfunction()

# Get nano specs option with nosys included between libc nano references
function(get_newlib_nano_specs_path out_var)
    _get_modified_specs_path(specs_path nano.specs add_nosys_to_lib)
    set(${out_var} "${specs_path}" PARENT_SCOPE)
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
        if(LIBC_PICOLIBC_NEWLIB_COMPATIBILITY)
            idf_toolchain_add_flags(COMPILE_OPTIONS "-D__STDC_WANT_LIB_EXT1__=0")
        endif()
    else()
        idf_toolchain_remove_flags(COMPILE_OPTIONS "-specs=.*picolibc.*")
    endif()

    if(CONFIG_LIBC_NEWLIB_NANO_FORMAT)
        get_newlib_nano_specs_path(nano_specs_path)
        idf_toolchain_add_flags(LINK_OPTIONS "\"--specs=${nano_specs_path}\"")
    else()
        idf_toolchain_remove_flags(LINK_OPTIONS "--specs=.*nano.*")
    endif()

    idf_toolchain_rerun_abi_detection()
else() # TODO IDF-14338
    if(CONFIG_STDATOMIC_S32C1I_SPIRAM_WORKAROUND)
        idf_build_set_property(COMPILE_OPTIONS "-mdisable-hardware-atomics" APPEND)
    endif()
endif()
