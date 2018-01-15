set(COMPONENT_ADD_INCLUDEDIRS port/include port/include/coap libcoap/include libcoap/include/coap)

set(COMPONENT_SRCS
  libcoap/src/address.c
  libcoap/src/async.c
  libcoap/src/block.c
  libcoap/src/coap_time.c
  libcoap/src/debug.c
  libcoap/src/encode.c
  libcoap/src/hashkey.c
  libcoap/src/mem.c
  libcoap/src/net.c
  libcoap/src/option.c
  libcoap/src/pdu.c
  libcoap/src/resource.c
  libcoap/src/str.c
  libcoap/src/subscribe.c
  libcoap/src/uri.c
  port/coap_io_socket.c
  )

register_component()

# Needed for coap headers in public builds, also.
#
# TODO: find a way to move this to a port header
target_compile_definitions(coap PUBLIC
	WITH_POSIX
)


