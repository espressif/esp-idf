idf_component_register(SRCS "main.c"
                    PRIV_REQUIRES bt bt_app_core_utils bredr_app_common_utils a2dp_sink_common_utils
                    PRIV_REQUIRES a2dp_sink_int_codec_utils avrcp_common_utils avrcp_abs_vol_utils
                    INCLUDE_DIRS ".")
