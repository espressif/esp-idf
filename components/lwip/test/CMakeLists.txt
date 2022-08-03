idf_component_register(SRC_DIRS "."
                    PRIV_REQUIRES test_utils)
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
