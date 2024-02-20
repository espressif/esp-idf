
set(srcs
    "test_app_main.c"
    "test_spi_slave.c"
    "test_spi_slave_queue.c"
)


# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(
    SRCS ${srcs}
    PRIV_REQUIRES test_utils driver test_driver_utils spi_flash
    WHOLE_ARCHIVE
)
