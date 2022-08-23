idf_component_register(SRCS "app_hf_msg_prs.c"
                            "app_hf_msg_set.c"
                            "bt_app_core.c"
                            "bt_app_hf.c"
                            "gpio_pcm_config.c"
                            "main.c"
                    INCLUDE_DIRS ".")
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
