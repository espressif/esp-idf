idf_component_register(
        SRCS test_spi_utils.c param_test.c test_dualboard_utils.c
        INCLUDE_DIRS include
        REQUIRES driver unity
    )
