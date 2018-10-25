set(COMPONENT_SRCS "ref_clock.c"
                   "test_runner.c"
                   "test_utils.c")
set(COMPONENT_ADD_INCLUDEDIRS include)

set(COMPONENT_REQUIRES spi_flash idf_test unity)

register_component()

