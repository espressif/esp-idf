set(srcs "test_app_main.c"
         "test_libgcc.c"
         "test_printf.c")

if(CONFIG_IDF_ENV_FPGA)
    list(APPEND srcs "test_miniz.c")
endif()

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES unity
                       WHOLE_ARCHIVE)
