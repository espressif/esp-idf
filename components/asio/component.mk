ifdef CONFIG_LWIP_IPV6

COMPONENT_ADD_INCLUDEDIRS := asio/asio/include port/include
COMPONENT_PRIV_INCLUDEDIRS := private_include
COMPONENT_SRCDIRS := asio/asio/src port/src

ifeq ($(CONFIG_ASIO_SSL_SUPPORT), )
COMPONENT_OBJEXCLUDE := asio/asio/src/asio_ssl.o port/src/esp_asio_openssl_stubs.o
endif

COMPONENT_SUBMODULES += asio

endif # CONFIG_LWIP_IPV6
