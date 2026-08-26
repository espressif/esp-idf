idf_component_register(SRCS "test_app_main.c"
                            "test_fmt_convert.c"
                            "test_calc_clk_div.c"
                            "test_hal_utils_misc.c"
                    INCLUDE_DIRS "."
                    REQUIRES unity hal
                    WHOLE_ARCHIVE)
