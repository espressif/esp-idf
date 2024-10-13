idf_component_register(SRCS "pthread_psram_tests.c"
                    INCLUDE_DIRS "."
                    PRIV_REQUIRES unity test_utils pthread) # note: esp_psram is set in the project's CMakeLists.txt
