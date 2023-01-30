set(app_sources "test_app_main.c" "test_ulp_riscv.c")
set(ulp_sources "ulp/test_main.c")
set(ulp_sources2 "ulp/test_main_second_cocpu_firmware.c")
set(ulp_sources3 "ulp/test_main_cocpu_crash.c")

idf_component_register(SRCS ${app_sources}
                       INCLUDE_DIRS "ulp"
                       REQUIRES ulp unity
                       WHOLE_ARCHIVE)

set(ulp_app_name ulp_test_app)
set(ulp_app_name2 ulp_test_app2)
set(ulp_app_name3 ulp_test_app3)
set(ulp_exp_dep_srcs ${app_sources})
ulp_embed_binary(${ulp_app_name} "${ulp_sources}" "${ulp_exp_dep_srcs}")
ulp_embed_binary(${ulp_app_name2} "${ulp_sources2}" "${ulp_exp_dep_srcs}")
ulp_embed_binary(${ulp_app_name3} "${ulp_sources3}" "${ulp_exp_dep_srcs}")
