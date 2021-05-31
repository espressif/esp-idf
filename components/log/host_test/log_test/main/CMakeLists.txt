idf_component_register(SRCS "log_test.cpp"
                    INCLUDE_DIRS
                    "."
                    $ENV{IDF_PATH}/tools/catch
                    REQUIRES log)
