idf_component_register(SRC_DIRS "."
                        PRIV_REQUIRES test_utils esp-tls unity
                        WHOLE_ARCHIVE)

# Expose esp-tls private headers for internal component tests
idf_component_get_property(esp_tls_dir esp-tls COMPONENT_DIR)
target_include_directories(${COMPONENT_LIB} PRIVATE "${esp_tls_dir}/private_include")
