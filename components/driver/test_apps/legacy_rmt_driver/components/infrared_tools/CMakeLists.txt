set(component_srcs "src/ir_builder_rmt_nec.c"
                   "src/ir_builder_rmt_rc5.c"
                   "src/ir_parser_rmt_nec.c"
                   "src/ir_parser_rmt_rc5.c")

idf_component_register(SRCS "${component_srcs}"
                       INCLUDE_DIRS "include"
                       PRIV_REQUIRES "driver")
