set(srcs "test_app_main.c"
         "test_rmt_common.c"
         "test_rmt_tx.c"
         "test_rmt_rx.c"
         "test_util_rmt_encoders.c")

idf_component_register(SRCS "${srcs}"
                       WHOLE_ARCHIVE)
