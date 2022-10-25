set(srcs "test_app_main.c"
         "test_rtc_clk.c")

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRCS ${srcs}
                       REQUIRES unity driver test_utils
                       WHOLE_ARCHIVE)
