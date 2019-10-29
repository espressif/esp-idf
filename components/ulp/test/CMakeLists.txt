if(IDF_TARGET STREQUAL "esp32")
    idf_component_register(SRC_DIRS esp32
                        INCLUDE_DIRS .
                        REQUIRES unity ulp soc esp_common)

    set(ulp_app_name ulp_test_app)
    set(ulp_s_sources "ulp/test_jumps_esp32.S")
    set(ulp_exp_dep_srcs "esp32/test_ulp_as.c")
    ulp_embed_binary(${ulp_app_name} ${ulp_s_sources} ${ulp_exp_dep_srcs})
endif()
