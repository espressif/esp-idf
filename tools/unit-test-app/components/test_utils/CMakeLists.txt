idf_component_register(SRCS "ref_clock.c"
                            "test_runner.c"
                            "test_utils.c"
                    INCLUDE_DIRS include
                    REQUIRES spi_flash idf_test unity)


