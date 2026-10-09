idf_component_register(SRCS "avrcp_psth_cmd_utils.c"
                            "avrcp_psth_cmd_service.c"
                    PRIV_REQUIRES bt avrcp_common_utils
                    INCLUDE_DIRS "." "../include")
