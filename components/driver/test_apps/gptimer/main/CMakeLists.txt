set(srcs "test_app_main.c"
         "test_gptimer.c"
         "test_gptimer_iram.c")

idf_component_register(SRCS ${srcs})

target_link_libraries(${COMPONENT_LIB} INTERFACE "-u test_app_include_gptimer" "-u test_app_include_gptimer_iram")
