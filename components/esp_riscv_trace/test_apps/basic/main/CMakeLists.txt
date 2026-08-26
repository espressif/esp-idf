set(srcs "test_app_main.c"
         "test_riscv_trace_common.c"
         "test_riscv_trace_basic.c")

if(CONFIG_SOC_RISCV_TRACE_FILTER_SUPPORTED)
    list(APPEND srcs "test_riscv_trace_filter.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES unity esp_riscv_trace esp_psram
                       WHOLE_ARCHIVE)
