set(app_sources "test_app_main.c" "test_lp_core.c")

# Add to P4 TODO IDF-7540
if(CONFIG_IDF_TARGET_ESP32C6)
    list(APPEND app_sources "test_lp_core_i2c.c")
endif()

set(lp_core_sources         "lp_core/test_main.c")
set(lp_core_sources_counter "lp_core/test_main_counter.c")
set(lp_core_sources_set_timer_wakeup "lp_core/test_main_set_timer_wakeup.c")
set(lp_core_sources_gpio "lp_core/test_main_gpio.c")

# Add to P4 TODO IDF-7540
if(CONFIG_IDF_TARGET_ESP32C6)
    set(lp_core_sources_i2c "lp_core/test_main_i2c.c")
endif()

idf_component_register(SRCS ${app_sources}
                       INCLUDE_DIRS "lp_core"
                       REQUIRES ulp unity esp_timer test_utils
                       WHOLE_ARCHIVE)

set(lp_core_exp_dep_srcs ${app_sources})

ulp_embed_binary(lp_core_test_app "${lp_core_sources}" "${lp_core_exp_dep_srcs}")
ulp_embed_binary(lp_core_test_app_counter "${lp_core_sources_counter}" "${lp_core_exp_dep_srcs}")
ulp_embed_binary(lp_core_test_app_set_timer_wakeup "${lp_core_sources_set_timer_wakeup}" "${lp_core_exp_dep_srcs}")
ulp_embed_binary(lp_core_test_app_gpio "${lp_core_sources_gpio}" "${lp_core_exp_dep_srcs}")

# Add to P4 TODO IDF-7540
if(CONFIG_IDF_TARGET_ESP32C6)
    ulp_embed_binary(lp_core_test_app_i2c "${lp_core_sources_i2c}" "${lp_core_exp_dep_srcs}")
endif()
