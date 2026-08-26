idf_component_register(SRCS "avrcp_abs_vol_utils.c"
                            "avrcp_abs_vol_service.c"
                    PRIV_REQUIRES bt
                    INCLUDE_DIRS "." "../include")
