COMPONENT_ADD_INCLUDEDIRS := asio/asio/include port/include
COMPONENT_PRIV_INCLUDEDIRS := private_include
COMPONENT_SRCDIRS := asio/asio/src
COMPONENT_OBJEXCLUDE := asio/asio/src/asio_ssl.o

COMPONENT_SUBMODULES += asio
