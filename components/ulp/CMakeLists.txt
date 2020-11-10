set(srcs "ulp.c"
        "ulp_macro.c")

if(CONFIG_ESP32S2_ULP_COPROC_RISCV)
        list(APPEND srcs "ulp_riscv.c")
endif()

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS include)
