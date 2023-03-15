set(srcs "app_main.c"
         "test_ecc.c")

idf_component_register(SRCS ${srcs}
                       REQUIRES unity
                       WHOLE_ARCHIVE)
