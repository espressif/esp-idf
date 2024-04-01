set(srcs)
set(public_include "include")

# JPEG related source files
if(CONFIG_SOC_JPEG_CODEC_SUPPORTED)
    list(APPEND srcs
                    "jpeg_common.c"
                    "jpeg_param.c"
        )
    if(CONFIG_SOC_JPEG_DECODE_SUPPORTED)
        list(APPEND srcs
                        "jpeg_parse_marker.c"
                        "jpeg_decode.c"
        )
    endif()
    if(CONFIG_SOC_JPEG_ENCODE_SUPPORTED)
        list(APPEND srcs
                        "jpeg_emit_marker.c"
                        "jpeg_encode.c"
        )
    endif()
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${public_include}
                       PRIV_REQUIRES "esp_mm" "esp_pm"
                      )
