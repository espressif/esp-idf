set(app_sources "test_app_main.c" "test_lp_core.c")
set(lp_core_sources "lp_core/test_main.c")

idf_component_register(SRCS ${app_sources}
                       INCLUDE_DIRS "lp_core"
                       REQUIRES ulp unity esp_timer
                       WHOLE_ARCHIVE)

set(lp_core_app_name lp_core_test_app)
set(lp_core_exp_dep_srcs ${app_sources})
ulp_embed_binary(${lp_core_app_name} "${lp_core_sources}" "${lp_core_exp_dep_srcs}")
