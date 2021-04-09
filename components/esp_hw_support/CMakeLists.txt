idf_build_get_property(target IDF_TARGET)

set(requires soc)
if(${target} STREQUAL "esp32")
     list(APPEND requires efuse)
endif()

set(priv_requires efuse)

set(srcs "compare_set.c" "cpu_util.c")
if(NOT BOOTLOADER_BUILD)
    list(APPEND srcs "esp_async_memcpy.c"
                     "esp_clk.c"
                     "clk_ctrl_os.c"
                     "hw_random.c"
                     "intr_alloc.c"
                     "mac_addr.c"
                     "sleep_modes.c")
     list(APPEND priv_requires esp_ipc)

     if(NOT CMAKE_BUILD_EARLY_EXPANSION)
          set_source_files_properties("${CMAKE_CURRENT_LIST_DIR}/sleep_modes.c" PROPERTIES
                                      COMPILE_FLAGS "-fno-jump-tables -fno-tree-switch-conversion")
     endif()
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS include include/soc
                       PRIV_INCLUDE_DIRS port/include
                       REQUIRES ${requires}
                       PRIV_REQUIRES ${priv_requires}
                       LDFRAGMENTS linker.lf)

idf_build_get_property(target IDF_TARGET)
add_subdirectory(port/${target})
