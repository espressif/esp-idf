idf_component_register(SRCS "avrcp_cover_art_utils.c"
                            "avrcp_cover_art_service.c"
                    PRIV_REQUIRES bt esp_lcd avrcp_common_utils
                    INCLUDE_DIRS "." "../include")
