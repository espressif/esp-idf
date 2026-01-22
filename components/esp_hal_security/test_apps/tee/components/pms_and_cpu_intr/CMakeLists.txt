cmake_minimum_required(VERSION 3.22)
idf_build_get_property(target IDF_TARGET)

set(srcs "src/common/test_apm_utils.c"
         "src/common/test_intr_utils.c"
         "src/common/test_setup_utils.c"
         "src/common/test_panic_handler.c")

list(APPEND srcs "src/pms/test_tee_sys_apm.c"
                 "src/pms/test_tee_vectors.S")
if(CONFIG_SOC_SUPPORT_TEE_PERI_APM_TEST)
    list(APPEND srcs "src/pms/test_tee_peri_apm.c")
endif()

if(CONFIG_SOC_SUPPORT_TEE_INTR_TEST)
    list(APPEND srcs "src/cpu_intr/test_interrupt.c"
                     "src/cpu_intr/test_vectors_m.S"
                     "src/cpu_intr/test_vectors_u.S")
endif()

idf_component_register(SRCS "${srcs}"
                       INCLUDE_DIRS "include" "priv_include"
                       PRIV_INCLUDE_DIRS "priv_include/${target}"
                       REQUIRES ulp unity esp_hal_security
                       PRIV_REQUIRES esp_hal_timg
                       LDFRAGMENTS linker.lf)

if(CONFIG_ULP_COPROC_ENABLED)
    set(ulp_app_name ulp_lp_core_${COMPONENT_NAME})
    set(ulp_rv_srcs "src/ulp/ulp_lp_core_main.c" "src/ulp/ulp_vectors.S")
    ulp_embed_binary(${ulp_app_name} "${ulp_rv_srcs}" "${ulp_exp_dep_srcs}")
endif()
