set(app_sources "test_app_main.c" "test_ulp_riscv.c" "test_ulp_riscv_i2c.c")
set(ulp_sources "ulp/test_main.c")
set(ulp_sources2 "ulp/test_main_second_cocpu_firmware.c")
set(ulp_sources3 "ulp/test_main_cocpu_crash.c")
set(ulp_sources4 "ulp/test_main_i2c.c")

idf_component_register(SRCS ${app_sources}
                       INCLUDE_DIRS "ulp"
                       REQUIRES ulp unity test_utils
                       WHOLE_ARCHIVE)

set(ulp_app_name ulp_test_app)
set(ulp_app_name2 ulp_test_app2)
set(ulp_app_name3 ulp_test_app3)
set(ulp_app_name4 ulp_test_app_i2c)
set(ulp_exp_dep_srcs ${app_sources})
ulp_embed_binary(${ulp_app_name} "${ulp_sources}" "${ulp_exp_dep_srcs}")
ulp_embed_binary(${ulp_app_name2} "${ulp_sources2}" "${ulp_exp_dep_srcs}")
ulp_embed_binary(${ulp_app_name3} "${ulp_sources3}" "${ulp_exp_dep_srcs}")
ulp_embed_binary(${ulp_app_name4} "${ulp_sources4}" "${ulp_exp_dep_srcs}")
