if(CONFIG_STDATOMIC_S32C1I_SPIRAM_WORKAROUND)
    idf_build_set_property(COMPILE_OPTIONS "-mdisable-hardware-atomics" APPEND)
endif()
