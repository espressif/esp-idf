set(srcs "app_main.c"
         "test_mpi.c")

idf_component_register(SRCS ${srcs}
                       REQUIRES unity
                       WHOLE_ARCHIVE)
