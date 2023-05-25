set(srcs "app_main.c")

if(CONFIG_SOC_MPI_SUPPORTED)
   list(APPEND srcs "mpi/test_mpi.c")
endif()

if(CONFIG_SOC_ECC_SUPPORTED)
   list(APPEND srcs "ecc/test_ecc.c")
endif()

if(CONFIG_SOC_HMAC_SUPPORTED)
   list(APPEND srcs "hmac/test_hmac.c")
endif()

if(CONFIG_SOC_DIG_SIGN_SUPPORTED)
   list(APPEND srcs "ds/test_ds.c")
endif()

if(CONFIG_SOC_ECDSA_SUPPORTED)
   list(APPEND srcs "ecdsa/test_ecdsa.c")
endif()

idf_component_register(SRCS ${srcs}
                       REQUIRES test_utils unity
                       WHOLE_ARCHIVE)
