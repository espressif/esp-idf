idf_build_get_property(target IDF_TARGET)

set(includes "include")

set(priv_requires heap spi_flash esp_mm)
if(${target} STREQUAL "esp32")
    list(APPEND priv_requires bootloader_support)
    # [refactor-todo]: requires "driver" for `spicommon_periph_claim`
    list(APPEND priv_requires driver)
endif()

set(srcs)

if(CONFIG_SPIRAM)
    list(APPEND srcs "esp_psram.c"
                     "mmu_psram_flash.c")

    if(${target} STREQUAL "esp32")
        list(APPEND srcs "esp32/esp_psram_extram_cache.c"
                         "esp32/esp_himem.c")
    endif()

    if(CONFIG_SPIRAM_MODE_QUAD)
        list(APPEND srcs "${target}/esp_psram_impl_quad.c")
    elseif(CONFIG_SPIRAM_MODE_OCT)
        list(APPEND srcs "${target}/esp_psram_impl_octal.c")
    endif()
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${includes}
                       PRIV_REQUIRES ${priv_requires}
                       LDFRAGMENTS linker.lf)

if(CONFIG_IDF_TARGET_ESP32 AND CONFIG_SPIRAM_CACHE_WORKAROUND AND NOT BOOTLOADER_BUILD)
    # Note: Adding as a PUBLIC compile option here causes this option to propagate to all
    # components that depend on esp_psram.
    #
    # To handle some corner cases, the same flag is set in project_include.cmake
    target_compile_options(${COMPONENT_LIB} PUBLIC -mfix-esp32-psram-cache-issue)
    # also, make sure we link with this option so correct toolchain libs are pulled in
    target_link_libraries(${COMPONENT_LIB} PUBLIC -mfix-esp32-psram-cache-issue)
    # set strategy selected
    # note that we don't need to set link options as the library linked is independent of this
    if(CONFIG_SPIRAM_CACHE_WORKAROUND_STRATEGY_DUPLDST)
        target_compile_options(${COMPONENT_LIB} PUBLIC -mfix-esp32-psram-cache-strategy=dupldst)
        target_link_libraries(${COMPONENT_LIB} PUBLIC -mfix-esp32-psram-cache-strategy=dupldst)
    endif()
    if(CONFIG_SPIRAM_CACHE_WORKAROUND_STRATEGY_MEMW)
        target_compile_options(${COMPONENT_LIB} PUBLIC -mfix-esp32-psram-cache-strategy=memw)
        target_link_libraries(${COMPONENT_LIB} PUBLIC -mfix-esp32-psram-cache-strategy=memw)
    endif()
    if(CONFIG_SPIRAM_CACHE_WORKAROUND_STRATEGY_NOPS)
        target_compile_options(${COMPONENT_LIB} PUBLIC -mfix-esp32-psram-cache-strategy=nops)
        target_link_libraries(${COMPONENT_LIB} PUBLIC -mfix-esp32-psram-cache-strategy=nops)
    endif()
endif()
