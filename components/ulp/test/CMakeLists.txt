idf_component_register(SRC_DIRS "."
                    INCLUDE_DIRS "."
                    REQUIRES unity ulp soc esp_common)

set(ulp_app_name ulp_test_app)
set(ulp_s_sources "ulp/test_jumps.S")
set(ulp_exp_dep_srcs "test_ulp_as.c")
ulp_embed_binary(${ulp_app_name} ${ulp_s_sources} ${ulp_exp_dep_srcs})
