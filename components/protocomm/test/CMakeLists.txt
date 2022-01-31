idf_component_register(SRC_DIRS "."
                    PRIV_INCLUDE_DIRS "."
                    PRIV_INCLUDE_DIRS "../proto-c/"
                    PRIV_REQUIRES cmock mbedtls protocomm protobuf-c test_utils)
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
