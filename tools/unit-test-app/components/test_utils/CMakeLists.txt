idf_component_register(SRCS "ref_clock_${CONFIG_IDF_TARGET}.c"
                            "test_runner.c"
                            "test_utils.c"
                    INCLUDE_DIRS include
                    REQUIRES spi_flash idf_test unity)


