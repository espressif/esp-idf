idf_build_get_property(target IDF_TARGET)

set(requires soc)
if(${target} STREQUAL "esp32")
     list(APPEND requires efuse)
endif()

set(srcs "compare_set.c" "cpu_util.c")
if(NOT BOOTLOADER_BUILD)
    list(APPEND srcs "esp_async_memcpy.c" "esp_clk.c" "clk_ctrl_os.c" "mac_addr.c" "hw_random.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS include include/soc
                       PRIV_INCLUDE_DIRS port/include
                       REQUIRES ${requires}
                       PRIV_REQUIRES efuse
                       LDFRAGMENTS linker.lf)

idf_build_get_property(target IDF_TARGET)
add_subdirectory(port/${target})
