idf_component_register(SRCS "a2dp_sink_int_codec_utils.c"
                            "audio_sink_service_idle.c"
                            "audio_sink_service_i2s.c"
                            "audio_sink_service_dac.c"
                    PRIV_REQUIRES bt esp_ringbuf driver
                    INCLUDE_DIRS "." "../include")
