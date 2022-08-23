idf_component_register(SRC_DIRS "."
                    INCLUDE_DIRS "."
                    REQUIRES cmock test_utils esp_hid)
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
