idf_component_register(SRC_DIRS "." "param_test"
                    INCLUDE_DIRS "include" "param_test/include"
                    REQUIRES unity test_utils driver nvs_flash)