if(CONFIG_IDF_TOOLCHAIN_GCC)
    # Remove all "-mfix-esp32-psram-cache*" from toolchain flags
    # that may have appeared during configuration changes.
    idf_toolchain_remove_flags(COMPILE_OPTIONS "-mfix-esp32-psram-cache")
    if(CONFIG_IDF_TARGET_ESP32 AND CONFIG_SPIRAM_CACHE_WORKAROUND AND NOT BOOTLOADER_BUILD)
        idf_toolchain_add_flags(COMPILE_OPTIONS "-mfix-esp32-psram-cache-issue")
        if(CONFIG_SPIRAM_CACHE_WORKAROUND_STRATEGY_DUPLDST)
            idf_toolchain_add_flags(COMPILE_OPTIONS "-mfix-esp32-psram-cache-strategy=dupldst")
        endif()
        if(CONFIG_SPIRAM_CACHE_WORKAROUND_STRATEGY_MEMW)
            idf_toolchain_add_flags(COMPILE_OPTIONS "-mfix-esp32-psram-cache-strategy=memw")
        endif()
        if(CONFIG_SPIRAM_CACHE_WORKAROUND_STRATEGY_NOPS)
            idf_toolchain_add_flags(COMPILE_OPTIONS "-mfix-esp32-psram-cache-strategy=nops")
        endif()
    endif()
else()
    if(CONFIG_IDF_TARGET_ESP32 AND CONFIG_SPIRAM_CACHE_WORKAROUND AND NOT BOOTLOADER_BUILD)
        # We do this here as well as in CMakeLists.txt, because targets that
        # are not part of the ESP-IDF build system (for cases where a generic
        # non-IDF CMakeLists.txt file is imported into a component) don't depend
        # on the esp32 component so don't get the extra flag. This handles that case.
        idf_build_set_property(COMPILE_OPTIONS "-mfix-esp32-psram-cache-issue" APPEND)
        if(CONFIG_SPIRAM_CACHE_WORKAROUND_STRATEGY_DUPLDST)
            idf_build_set_property(COMPILE_OPTIONS "-mfix-esp32-psram-cache-strategy=dupldst" APPEND)
        endif()
        if(CONFIG_SPIRAM_CACHE_WORKAROUND_STRATEGY_MEMW)
            idf_build_set_property(COMPILE_OPTIONS "-mfix-esp32-psram-cache-strategy=memw" APPEND)
        endif()
        if(CONFIG_SPIRAM_CACHE_WORKAROUND_STRATEGY_NOPS)
            idf_build_set_property(COMPILE_OPTIONS "-mfix-esp32-psram-cache-strategy=nops" APPEND)
        endif()
    endif()
endif()
