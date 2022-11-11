idf_component_register(
        SRCS test_spi_utils.c param_test.c
        INCLUDE_DIRS include
        REQUIRES driver unity
    )
