if(CONFIG_SPIRAM_CACHE_WORKAROUND)
    # We do this here as well as in CMakeLists.txt, because targets that
    # are not part of the ESP-IDF build system (for cases where a generic
    # non-IDF CMakeLists.txt file is imported into a component) don't depend
    # on the esp32 component so don't get the extra flag. This handles that case.
    add_compile_options(-mfix-esp32-psram-cache-issue)
endif()

