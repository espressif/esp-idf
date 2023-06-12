set(srcs "test_app_main.c"
         "test_twai_loop_back.c"
         "test_twai_interactive.c")

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES unity driver
                       WHOLE_ARCHIVE)
