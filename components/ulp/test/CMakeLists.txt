if(IDF_TARGET STREQUAL "esp32")
    set(src_dirs "ulp_fsm")
    set(ulp_sources "ulp_fsm/ulp/test_jumps.S")
elseif(IDF_TARGET STREQUAL "esp32s2" OR IDF_TARGET STREQUAL "esp32s3")
    if(CONFIG_ESP32S2_ULP_COPROC_RISCV OR CONFIG_ESP32S3_ULP_COPROC_RISCV)
        set(src_dirs "ulp_riscv")
        set(ulp_sources "ulp_riscv/ulp/test_main.c")
    else()
        set(src_dirs "ulp_fsm")
        set(ulp_sources "ulp_fsm/ulp/test_jumps.S")
    endif()
endif()

idf_component_register(SRC_DIRS ${src_dirs}
PRIV_INCLUDE_DIRS .
PRIV_REQUIRES cmock ulp soc esp_common driver)

set(ulp_app_name ulp_test_app)
set(ulp_exp_dep_srcs ${src_dirs})
ulp_embed_binary(${ulp_app_name} "${ulp_sources}" "${ulp_exp_dep_srcs}")
