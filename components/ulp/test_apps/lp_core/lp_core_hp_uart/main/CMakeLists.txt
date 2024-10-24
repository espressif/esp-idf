set(app_sources "test_app_main.c" "test_lp_core.c")
set(lp_core_sources             "lp_core/test_hello_main.c")
set(lp_core_sources_panic       "lp_core/test_panic_main.c")
set(lp_core_sources_shared_mem  "lp_core/test_shared_mem_main.c")
set(lp_core_sources_lp_rom              "lp_core/test_lp_rom_main.c")

idf_component_register(SRCS ${app_sources}
                       INCLUDE_DIRS "lp_core"
                       REQUIRES ulp unity esp_timer test_utils
                       WHOLE_ARCHIVE)

set(lp_core_exp_dep_srcs ${app_sources})

ulp_embed_binary(lp_core_test_app "${lp_core_sources}" "${lp_core_exp_dep_srcs}")
ulp_embed_binary(lp_core_test_app_panic "${lp_core_sources_panic}" "${lp_core_exp_dep_srcs}")
ulp_embed_binary(lp_core_test_app_shared_mem "${lp_core_sources_shared_mem}" "${lp_core_exp_dep_srcs}")

if(CONFIG_ESP_ROM_HAS_LP_ROM)
    ulp_embed_binary(lp_core_test_app_lp_rom "${lp_core_sources_lp_rom}" "${lp_core_exp_dep_srcs}")
endif()
