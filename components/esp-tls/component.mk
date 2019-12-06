
COMPONENT_SRCDIRS := .
COMPONENT_OBJS := esp_tls.o

COMPONENT_ADD_INCLUDEDIRS := . private_include


ifneq ($(CONFIG_ESP_TLS_USING_MBEDTLS), )
COMPONENT_OBJS += esp_tls_mbedtls.o
endif

ifneq ($(CONFIG_ESP_TLS_USING_WOLFSSL), )
COMPONENT_OBJS += esp_tls_wolfssl.o
endif

CFLAGS += -DWOLFSSL_USER_SETTINGS
