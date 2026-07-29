# Sub-projects with a custom toolchain (e.g. ULP) do not use the IDF
# toolchain response file machinery. Skip flag manipulation.
if(IDF_CUSTOM_TOOLCHAIN)
    return()
endif()

if(CONFIG_IDF_TOOLCHAIN_GCC)
    if(CONFIG_STDATOMIC_S32C1I_SPIRAM_WORKAROUND)
        idf_toolchain_add_flags(COMPILE_OPTIONS "-mdisable-hardware-atomics")
    else()
        idf_toolchain_remove_flags(COMPILE_OPTIONS "-mdisable-hardware-atomics")
    endif()

    idf_toolchain_remove_flags(COMPILE_OPTIONS "-specs=picolibc.specs")
    idf_toolchain_remove_flags(COMPILE_OPTIONS "-D__STDC_WANT_LIB_EXT1__=0")
    # TODO GCC-495:remove
    idf_toolchain_remove_flags(COMPILE_OPTIONS "-D__PICOLIBC_ERRNO_FUNCTION=__errno")
    if(CONFIG_LIBC_PICOLIBC)
        idf_toolchain_add_flags(COMPILE_OPTIONS "-specs=picolibc.specs")
        # TODO GCC-495:remove
        idf_toolchain_add_flags(COMPILE_OPTIONS "-D__PICOLIBC_ERRNO_FUNCTION=__errno")
        if(CONFIG_LIBC_PICOLIBC_NEWLIB_COMPATIBILITY)
            idf_toolchain_add_flags(COMPILE_OPTIONS "-D__STDC_WANT_LIB_EXT1__=0")
        endif()
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
