idf_component_register(SRCS "esp_timer_test.cpp"
                    INCLUDE_DIRS
                    "."
                    $ENV{IDF_PATH}/tools/catch
                    REQUIRES cmock esp_timer experimental_cpp_component)
