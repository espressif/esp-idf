idf_component_register(SRCS "esp_event_test.cpp"
                    INCLUDE_DIRS "../../" $ENV{IDF_PATH}/tools/catch
                    REQUIRES esp_event cmock)
