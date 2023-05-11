set(srcs "app_main.c")

if(CONFIG_SOC_MPI_SUPPORTED)
   list(APPEND srcs "mpi/test_mpi.c")
endif()


idf_component_register(SRCS ${srcs}
                       REQUIRES test_utils unity
                       WHOLE_ARCHIVE)
