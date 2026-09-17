if(CONFIG_IDF_TOOLCHAIN_GCC)
    idf_toolchain_remove_flags(COMPILE_OPTIONS "-mdisable-hardware-atomics"
                                               "-specs=picolibc.specs"
                                               "-D__STDC_WANT_LIB_EXT1__=0"
                                               "--specs=nano.specs")

    if(CONFIG_STDATOMIC_S32C1I_SPIRAM_WORKAROUND)
        idf_toolchain_add_flags(COMPILE_OPTIONS "-mdisable-hardware-atomics")
    endif()

    if(CONFIG_LIBC_PICOLIBC)
        idf_toolchain_add_flags(COMPILE_OPTIONS "-specs=picolibc.specs")
        if(LIBC_PICOLIBC_NEWLIB_COMPATIBILITY)
            idf_toolchain_add_flags(COMPILE_OPTIONS "-D__STDC_WANT_LIB_EXT1__=0")
        endif()
    endif()

    if(CONFIG_LIBC_NEWLIB_NANO_FORMAT)
        idf_toolchain_add_flags(LINK_OPTIONS "--specs=nano.specs")
    endif()

    idf_toolchain_rerun_abi_detection()
else() # TODO IDF-14338
    if(CONFIG_STDATOMIC_S32C1I_SPIRAM_WORKAROUND)
        idf_build_set_property(COMPILE_OPTIONS "-mdisable-hardware-atomics" APPEND)
    endif()
endif()
