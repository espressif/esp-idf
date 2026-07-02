set(app_sources "test_app_main.c" "test_lp_gpio_wakeup.c")
set(lp_core_sources "lp_core/test_main_gpio_wakeup.c")

idf_component_register(SRCS ${app_sources}
                       REQUIRES ulp unity test_utils esp_driver_gpio
                       WHOLE_ARCHIVE)

set(lp_core_exp_dep_srcs ${app_sources})

ulp_embed_binary(lp_core_test_app_gpio_wakeup "${lp_core_sources}" "${lp_core_exp_dep_srcs}")
