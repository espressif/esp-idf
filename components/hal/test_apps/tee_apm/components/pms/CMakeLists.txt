cmake_minimum_required(VERSION 3.16)
idf_build_get_property(target IDF_TARGET)

set(srcs "src/test_tee_vectors.S"
         "src/test_panic_handler.c"
         "src/test_intr_utils.c"
         "src/test_apm_utils.c"
         "src/test_setup_utils.c"
         "src/test_tee_sys_apm.c")
if(CONFIG_SOC_APM_SUPPORT_TEE_PERI_ACCESS_CTRL)
    list(APPEND srcs "src/test_tee_peri_apm.c")
endif()

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "include" "priv_include"
                       PRIV_INCLUDE_DIRS "priv_include/${target}"
                       REQUIRES ulp unity
                       LDFRAGMENTS linker.lf)

if(CONFIG_ULP_COPROC_ENABLED)
    set(ulp_app_name ulp_lp_core_${COMPONENT_NAME})
    set(ulp_rv_srcs "src/ulp/ulp_lp_core_main.c" "src/ulp/ulp_vectors.S")
    set(ulp_exp_dep_srcs "src/test_tee_apm_pms.c")
    ulp_embed_binary(${ulp_app_name} "${ulp_rv_srcs}" "${ulp_exp_dep_srcs}")
endif()
