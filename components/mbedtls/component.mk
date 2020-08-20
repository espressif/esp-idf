#
# Component Makefile
#

COMPONENT_ADD_INCLUDEDIRS := port/include mbedtls/include

COMPONENT_SRCDIRS := mbedtls/library port port/$(IDF_TARGET)

COMPONENT_OBJEXCLUDE := mbedtls/library/net_sockets.o

COMPONENT_SUBMODULES += mbedtls

ifndef CONFIG_MBEDTLS_HARDWARE_MPI
    COMPONENT_OBJEXCLUDE += port/$(IDF_TARGET)/esp_bignum.o
endif

ifndef CONFIG_MBEDTLS_HARDWARE_SHA
    COMPONENT_OBJEXCLUDE += port/esp_sha1.o port/esp_sha256.o port/esp_sha512.o port/$(IDF_TARGET)/sha.o
endif
