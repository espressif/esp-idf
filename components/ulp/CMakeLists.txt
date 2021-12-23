idf_build_get_property(target IDF_TARGET)

set(srcs "")
set(includes include)

if(CONFIG_SOC_ULP_SUPPORTED)
    list(APPEND srcs
        "ulp.c"
        "ulp_macro.c")

    if(CONFIG_SOC_RISCV_COPROC_SUPPORTED)
        list(APPEND srcs
            "ulp_riscv.c")
    endif()
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${includes})
