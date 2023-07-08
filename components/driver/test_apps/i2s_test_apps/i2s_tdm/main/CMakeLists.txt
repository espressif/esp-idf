idf_component_register(SRCS "test_app_main.c" "test_i2s_tdm_full_duplex.c"
                       PRIV_REQUIRES unity driver test_utils
                       INCLUDE_DIRS "."
                       WHOLE_ARCHIVE
)
