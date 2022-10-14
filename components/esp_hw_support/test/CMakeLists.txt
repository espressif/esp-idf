idf_component_register(SRC_DIRS "."
                    PRIV_INCLUDE_DIRS "${include_dirs}"
                    PRIV_REQUIRES cmock test_utils esp_hw_support driver efuse esp_timer esp_psram spi_flash)
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")

target_link_libraries(${COMPONENT_LIB} INTERFACE "-u ld_include_test_dport_xt_highint5")
