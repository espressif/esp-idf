#
# Component Makefile
#

ifdef CONFIG_LWIP_IPV6

COMPONENT_ADD_INCLUDEDIRS := port/include port/include libcoap/include

COMPONENT_OBJS = libcoap/src/address.o libcoap/src/async.o libcoap/src/block.o libcoap/src/coap_asn1.o libcoap/src/coap_cache.o libcoap/src/coap_debug.o libcoap/src/coap_event.o libcoap/src/coap_hashkey.o libcoap/src/coap_io.o libcoap/src/coap_mbedtls.o libcoap/src/coap_notls.o libcoap/src/coap_prng.o libcoap/src/coap_session.o libcoap/src/coap_tcp.o libcoap/src/coap_time.o libcoap/src/encode.o libcoap/src/mem.o libcoap/src/net.o libcoap/src/option.o libcoap/src/pdu.o libcoap/src/resource.o libcoap/src/str.o libcoap/src/subscribe.o libcoap/src/uri.o

COMPONENT_SRCDIRS := libcoap/src libcoap port

COMPONENT_SUBMODULES += libcoap

# Silence format truncation warning, until it is fixed upstream
# libcoap/src/coap_debug.o: CFLAGS += -Wno-format-truncation

endif  # CONFIG_LWIP_IPV6
