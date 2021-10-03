idf_build_get_property(target IDF_TARGET)

set(requires soc)
set(priv_requires efuse bootloader_support spi_flash)
if(${target} STREQUAL "esp32")
     list(APPEND requires efuse)
endif()

set(srcs "compare_set.c" "cpu_util.c")
if(NOT BOOTLOADER_BUILD)
    list(APPEND srcs "esp_async_memcpy.c"
                     "esp_clk.c"
                     "clk_ctrl_os.c"
                     "hw_random.c"
                     "intr_alloc.c"
                     "mac_addr.c"
                     "sleep_modes.c"
                     "sleep_gpio.c"
                     "sleep_mac_bb.c"
                     "regi2c_ctrl.c")
    if(NOT CONFIG_IDF_TARGET_ESP32 AND NOT CONFIG_IDF_TARGET_ESP32S2)
        list(APPEND srcs "sleep_retention.c")
    endif()
    list(APPEND requires esp_ipc)
else()
    # Requires "_esp_error_check_failed()" function
    list(APPEND priv_requires "esp_system")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS include include/soc include/soc/${target}
                       PRIV_INCLUDE_DIRS port/include
                       REQUIRES ${requires}
                       PRIV_REQUIRES "${priv_requires}"
                       LDFRAGMENTS linker.lf)

idf_build_get_property(target IDF_TARGET)
add_subdirectory(port/${target})

if(CONFIG_IDF_TARGET_ESP32 AND CONFIG_SPIRAM_CACHE_WORKAROUND AND NOT BOOTLOADER_BUILD)
    # Note: Adding as a PUBLIC compile option here causes this option to propagate to all
    # components that depend on esp32.
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
