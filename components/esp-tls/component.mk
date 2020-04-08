
COMPONENT_SRCDIRS := . esp-tls-crypto
COMPONENT_OBJS := esp_tls.o esp-tls-crypto/esp_tls_crypto.o esp_tls_error_capture.o

COMPONENT_ADD_INCLUDEDIRS := . esp-tls-crypto private_include


ifneq ($(CONFIG_ESP_TLS_USING_MBEDTLS), )
COMPONENT_OBJS += esp_tls_mbedtls.o
endif

ifneq ($(CONFIG_ESP_TLS_USING_WOLFSSL), )
COMPONENT_OBJS += esp_tls_wolfssl.o
endif

CFLAGS += -DWOLFSSL_USER_SETTINGS
