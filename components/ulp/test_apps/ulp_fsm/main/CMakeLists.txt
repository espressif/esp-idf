set(app_sources "test_app_main.c" "test_ulp.c" "test_ulp_manual.c")
set(ulp_sources "ulp/test_jumps.S")

idf_component_register(SRCS ${app_sources}
                       REQUIRES ulp unity
                       WHOLE_ARCHIVE)

set(ulp_app_name ulp_test_app)
set(ulp_exp_dep_srcs ${app_sources})
ulp_embed_binary(${ulp_app_name} "${ulp_sources}" "${ulp_exp_dep_srcs}")
