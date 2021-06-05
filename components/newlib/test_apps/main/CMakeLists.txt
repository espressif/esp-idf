idf_component_register(SRCS
                        "test_newlib_main.c"
                        "test_stdatomic.c"
                       REQUIRES test_utils
                       PRIV_REQUIRES unity)
