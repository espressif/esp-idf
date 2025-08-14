idf_component_register(SRCS "a2dp_sink_int_codec_utils.c"
                            "audio_sink_service_idle.c"
                            "audio_sink_service_i2s.c"
                            "audio_sink_service_dac.c"
                    PRIV_REQUIRES esp_driver_i2s bt esp_ringbuf esp_driver_dac
                    INCLUDE_DIRS "." "../include")
