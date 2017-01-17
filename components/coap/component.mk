#
# Component Makefile
#

COMPONENT_ADD_INCLUDEDIRS := port/include port/include/coap libcoap/include libcoap/include/coap

COMPONENT_OBJS = libcoap/src/address.o libcoap/src/async.o libcoap/src/block.o libcoap/src/coap_time.o libcoap/src/debug.o libcoap/src/encode.o libcoap/src/hashkey.o libcoap/src/mem.o libcoap/src/net.o libcoap/src/option.o libcoap/src/pdu.o libcoap/src/resource.o libcoap/src/str.o libcoap/src/subscribe.o libcoap/src/uri.o port/coap_io_socket.o

COMPONENT_SRCDIRS := libcoap/src libcoap port

COMPONENT_SUBMODULES += libcoap
