idf_component_register(SRC_DIRS .
                       PRIV_REQUIRES unity esp_pm ulp driver esp_timer test_utils)
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
