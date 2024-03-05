idf_component_register(
    SRCS "cdc_acm_vcp_example_main.cpp"
    INCLUDE_DIRS "."
    )

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-missing-field-initializers")
