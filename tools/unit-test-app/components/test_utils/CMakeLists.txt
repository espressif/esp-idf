idf_component_register(SRCS "ccomp_timer.c"
                            "ref_clock.c"
                            "test_runner.c"
                            "test_utils.c"
                            "ccomp_timer_impl.c"
                    INCLUDE_DIRS include
                    PRIV_INCLUDE_DIRS private_include
                    REQUIRES spi_flash idf_test unity
                    PRIV_REQUIRES perfmon esp_ipc)


