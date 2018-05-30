#
# Component Makefile
#

COMPONENT_ADD_INCLUDEDIRS := port/include mbedtls/include

COMPONENT_SRCDIRS := mbedtls/library port

COMPONENT_OBJEXCLUDE := mbedtls/library/net_sockets.o

COMPONENT_SUBMODULES += mbedtls

