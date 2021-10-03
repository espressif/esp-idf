if(IDF_TARGET STREQUAL "esp32s3")
    return()
endif()
idf_component_register(SRC_DIRS ${IDF_TARGET}
PRIV_INCLUDE_DIRS .
PRIV_REQUIRES cmock ulp soc esp_common driver)

if(IDF_TARGET STREQUAL "esp32")
    set(ulp_sources "ulp/test_jumps_esp32.S")
elseif(IDF_TARGET STREQUAL "esp32s2")
    set(ulp_sources "ulp_riscv/test_main.c")
endif()

set(ulp_app_name ulp_test_app)
set(ulp_exp_dep_srcs ${IDF_TARGET})
ulp_embed_binary(${ulp_app_name} "${ulp_sources}" "${ulp_exp_dep_srcs}")
