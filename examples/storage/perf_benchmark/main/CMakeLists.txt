idf_component_register(SRCS "perf_benchmark_example_main.c" "perf_benchmark_example_tests.c"
    "perf_benchmark_example_sd_utils.c"
                       PRIV_REQUIRES fatfs spiffs
                       INCLUDE_DIRS ".")
