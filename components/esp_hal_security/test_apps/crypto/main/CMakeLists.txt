set(srcs "app_main.c")
set(priv_include_dirs ".")

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

if(CONFIG_CRYPTO_IS_KEY_MANAGER_SUPPORTED)
   list(APPEND srcs "key_manager/test_key_manager.c"
                    "$ENV{IDF_PATH}/components/esp_security/src/esp_key_mgr.c")
   list(APPEND priv_include_dirs "$ENV{IDF_PATH}/components/esp_security/include")
endif()

if(CONFIG_SOC_AES_SUPPORTED)
   list(APPEND srcs "aes/test_aes.c"
                     "$ENV{IDF_PATH}/components/mbedtls/port/aes/esp_aes_common.c"
                     "$ENV{IDF_PATH}/components/mbedtls/port/aes/esp_aes.c")

   list(APPEND priv_include_dirs "$ENV{IDF_PATH}/components/mbedtls/port/include"
                                 "$ENV{IDF_PATH}/components/mbedtls/port/aes/include")

   if(CONFIG_SOC_AES_SUPPORT_DMA)
      list(APPEND priv_include_dirs "$ENV{IDF_PATH}/components/mbedtls/port/aes/dma/include")
      if(NOT CONFIG_SOC_AES_GDMA)
         list(APPEND srcs "$ENV{IDF_PATH}/components/mbedtls/port/aes/dma/esp_aes_crypto_dma_impl.c")
      else()
         list(APPEND srcs "$ENV{IDF_PATH}/components/mbedtls/port/aes/dma/esp_aes_gdma_impl.c"
                           "$ENV{IDF_PATH}/components/mbedtls/port/crypto_shared_gdma/esp_crypto_shared_gdma.c")
      endif()

      if(CONFIG_SOC_AES_SUPPORT_GCM)
         list(APPEND srcs "$ENV{IDF_PATH}/components/mbedtls/port/aes/esp_aes_gcm.c")
      endif()
   endif()
endif()

if(CONFIG_SOC_SHA_SUPPORTED)
   if(NOT CONFIG_SOC_SHA_SUPPORT_PARALLEL_ENG)
      list(APPEND srcs "sha/test_sha.c"
                        "sha/sha_block.c")
      list(APPEND priv_include_dirs "sha/include"
                                    "$ENV{IDF_PATH}/components/mbedtls/port/include")

      if(CONFIG_SOC_SHA_SUPPORT_DMA)
         list(APPEND srcs "sha/sha_dma.c"
                           "$ENV{IDF_PATH}/components/mbedtls/port/sha/core/sha.c")
         list(APPEND priv_include_dirs "$ENV{IDF_PATH}/components/mbedtls/port/sha/core/include")

         if(NOT CONFIG_SOC_SHA_GDMA)
            list(APPEND srcs "$ENV{IDF_PATH}/components/mbedtls/port/sha/core/esp_sha_crypto_dma_impl.c")
         else()
            list(APPEND srcs "$ENV{IDF_PATH}/components/mbedtls/port/sha/core/esp_sha_gdma_impl.c"
                              "$ENV{IDF_PATH}/components/mbedtls/port/crypto_shared_gdma/esp_crypto_shared_gdma.c")
         endif()
      endif()
   endif()
endif()

if(CONFIG_SOC_FLASH_ENCRYPTION_XTS_AES)
   list(APPEND srcs "xts_aes/test_xts_aes.c")
endif()

idf_component_register(SRCS ${srcs}
                       PRIV_REQUIRES efuse mbedtls esp_security esp_mm bootloader_support spi_flash esp_driver_dma
                       REQUIRES test_utils unity
                       WHOLE_ARCHIVE
                       PRIV_INCLUDE_DIRS "${priv_include_dirs}"
                       )
