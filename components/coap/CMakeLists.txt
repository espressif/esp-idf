set(include_dirs port/include port/include/coap libcoap/include libcoap/include/coap2)

set(srcs 
    "libcoap/src/address.c"
    "libcoap/src/async.c"
    "libcoap/src/block.c"
    "libcoap/src/coap_event.c"
    "libcoap/src/coap_hashkey.c"
    "libcoap/src/coap_session.c"
    "libcoap/src/coap_time.c"
    "port/coap_debug.c"
    "libcoap/src/encode.c"
    "libcoap/src/mem.c"
    "libcoap/src/net.c"
    "libcoap/src/option.c"
    "libcoap/src/pdu.c"
    "libcoap/src/resource.c"
    "libcoap/src/str.c"
    "libcoap/src/subscribe.c"
    "libcoap/src/uri.c"
    "libcoap/src/coap_io.c"
    "port/coap_mbedtls.c")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "${include_dirs}"
                    REQUIRES lwip mbedtls)

# Silence format truncation warning, until it is fixed upstream
set_source_files_properties(port/coap_debug.c PROPERTIES COMPILE_FLAGS -Wno-format-truncation)

# Needed for coap headers in public builds, also.
#
# TODO: find a way to move this to a port header
target_compile_definitions(${COMPONENT_LIB} PUBLIC WITH_POSIX)

