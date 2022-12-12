idf_component_register(SRCS "lwip_test.c"
                       REQUIRES test_utils
                       INCLUDE_DIRS "."
                       PRIV_REQUIRES unity lwip test_utils)
