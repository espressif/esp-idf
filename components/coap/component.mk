#
# Component Makefile
#

COMPONENT_ADD_INCLUDEDIRS := port/include port/include/coap libcoap/include libcoap/include/coap2

COMPONENT_OBJS = libcoap/src/address.o libcoap/src/async.o libcoap/src/block.o libcoap/src/coap_event.o libcoap/src/coap_hashkey.o libcoap/src/coap_session.o libcoap/src/coap_time.o libcoap/src/coap_debug.o libcoap/src/encode.o libcoap/src/mem.o libcoap/src/net.o libcoap/src/option.o libcoap/src/pdu.o libcoap/src/resource.o libcoap/src/str.o libcoap/src/subscribe.o libcoap/src/uri.o libcoap/src/coap_notls.o port/coap_io.o

COMPONENT_SRCDIRS := libcoap/src libcoap port

COMPONENT_SUBMODULES += libcoap

# Silence format truncation warning, until it is fixed upstream
libcoap/src/coap_debug.o: CFLAGS += -Wno-format-truncation
