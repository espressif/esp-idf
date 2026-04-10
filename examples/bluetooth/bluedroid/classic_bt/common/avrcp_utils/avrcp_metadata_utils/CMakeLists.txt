idf_component_register(SRCS "avrcp_metadata_utils.c"
                            "avrcp_metadata_service.c"
                    PRIV_REQUIRES bt avrcp_common_utils
                    INCLUDE_DIRS "." "../include")
