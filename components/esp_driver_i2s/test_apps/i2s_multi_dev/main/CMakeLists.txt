idf_component_register(SRCS "test_app_main.c" "test_i2s_multi_dev.c"
                       PRIV_REQUIRES unity driver test_utils
                       INCLUDE_DIRS "."
                       WHOLE_ARCHIVE
)
