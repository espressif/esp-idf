idf_component_register(SRC_DIRS "."
                    PRIV_INCLUDE_DIRS "${include_dirs}"
                    PRIV_REQUIRES cmock test_utils)
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
