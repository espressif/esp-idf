idf_component_register(SRCS "test_bt_main.c"
                            "test_bt_common.c"
                            "test_smp.c"
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES unity bt
                       WHOLE_ARCHIVE)
