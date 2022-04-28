if(IDF_TARGET STREQUAL "esp32c2")
    # ESP32-C2 does't support rmt
    return()
endif()

set(component_srcs "src/ir_builder_rmt_nec.c"
                   "src/ir_builder_rmt_rc5.c"
                   "src/ir_parser_rmt_nec.c"
                   "src/ir_parser_rmt_rc5.c")

idf_component_register(SRCS "${component_srcs}"
                       INCLUDE_DIRS "include"
                       PRIV_INCLUDE_DIRS ""
                       PRIV_REQUIRES "driver"
                       REQUIRES "")
