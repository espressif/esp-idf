idf_component_register(SRCS
                        "test_newlib_main.c"
                        "test_stdatomic.c"
                        "test_misc.c"
                        "test_file.c"
                       REQUIRES test_utils
                       PRIV_REQUIRES unity vfs)
