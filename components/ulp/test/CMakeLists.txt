set(COMPONENT_SRCDIRS ".")
set(COMPONENT_ADD_INCLUDEDIRS ".")

set(COMPONENT_REQUIRES unity ulp soc esp_common)

register_component()

set(ulp_app_name ulp_test_app)
set(ulp_s_sources "ulp/test_jumps.S")
set(ulp_exp_dep_srcs "test_ulp_as.c")
ulp_embed_binary(${ulp_app_name} ${ulp_s_sources} ${ulp_exp_dep_srcs})
