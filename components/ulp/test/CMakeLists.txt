if(IDF_TARGET STREQUAL "esp32s2" OR IDF_TARGET STREQUAL "esp32s3")
    #Disabled for now due to RTC bug IDF-4801
    return()
endif()

if(CONFIG_ULP_COPROC_TYPE_FSM)

    set(src_dirs "ulp_fsm")
    set(ulp_sources "ulp_fsm/ulp/test_jumps.S")

elseif(CONFIG_ULP_COPROC_TYPE_RISCV)

    set(src_dirs "ulp_riscv")
    set(ulp_sources "ulp_riscv/ulp/test_main.c")

endif()

idf_component_register(SRC_DIRS ${src_dirs}
PRIV_INCLUDE_DIRS .
PRIV_REQUIRES cmock ulp soc esp_common driver)

set(ulp_app_name ulp_test_app)
set(ulp_exp_dep_srcs ${src_dirs})
ulp_embed_binary(${ulp_app_name} "${ulp_sources}" "${ulp_exp_dep_srcs}")
