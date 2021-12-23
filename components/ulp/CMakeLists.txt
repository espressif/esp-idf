idf_build_get_property(target IDF_TARGET)

if(NOT (IDF_TARGET STREQUAL "esp32c3") AND NOT (IDF_TARGET STREQUAL "esp32h2"))
    set(srcs "ulp.c"
             "ulp_macro.c")

    if(CONFIG_ESP32S2_ULP_COPROC_RISCV OR CONFIG_ESP32S3_ULP_COPROC_RISCV)
            list(APPEND srcs "ulp_riscv.c")
    endif()

    idf_component_register(SRCS ${srcs}
                           INCLUDE_DIRS include)
endif()
