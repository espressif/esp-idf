set(MY_COMPONENT_REQUIRES
    bt_app_core_utils
    bredr_app_common_utils
    a2dp_sink_common_utils
    a2dp_sink_int_codec_utils
    avrcp_common_utils
    avrcp_metadata_utils
    avrcp_abs_vol_utils
)

idf_component_register(SRCS "bt_app_av.c"
                            "main.c"
                    PRIV_REQUIRES bt nvs_flash
                    PRIV_REQUIRES ${MY_COMPONENT_REQUIRES}
                    INCLUDE_DIRS ".")
