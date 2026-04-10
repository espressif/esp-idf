if(CONFIG_EXAMPLE_A2DP_SINK_STREAM_ENABLE)
    if(CONFIG_EXAMPLE_A2DP_SINK_USE_EXTERNAL_CODEC)
        set(EXTRA_COMPONENTS a2dp_sink_ext_codec_utils)
    else()
        set(EXTRA_COMPONENTS a2dp_sink_int_codec_utils)
    endif()
endif()

idf_component_register(SRCS "bt_app_av.c"
                            "main.c"
                    PRIV_REQUIRES bt bt_app_core_utils bredr_app_common_utils a2dp_sink_common_utils
                    PRIV_REQUIRES ${EXTRA_COMPONENTS} avrcp_common_utils avrcp_metadata_utils avrcp_cover_art_utils
                    INCLUDE_DIRS ".")
