if(NOT CONFIG_LWIP_IPV6 AND NOT CMAKE_BUILD_EARLY_EXPANSION)
    message(STATUS "IPV6 support is disabled so the coap component will not be built")
    # note: the component is still included in the build so it can become visible again in config
    # without needing to re-run CMake. However no source or header files are built.
    idf_component_register()
    return()
endif()

set(include_dirs port/include port/include libcoap/include)

set(srcs
    "libcoap/src/address.c"
    "libcoap/src/async.c"
    "libcoap/src/block.c"
    "libcoap/src/coap_asn1.c"
    "libcoap/src/coap_cache.c"
    "libcoap/src/coap_debug.c"
    "libcoap/src/coap_event.c"
    "libcoap/src/coap_hashkey.c"
    "libcoap/src/coap_io.c"
    "libcoap/src/coap_mbedtls.c"
    "libcoap/src/coap_notls.c"
    "libcoap/src/coap_prng.c"
    "libcoap/src/coap_session.c"
    "libcoap/src/coap_tcp.c"
    "libcoap/src/coap_time.c"
    "libcoap/src/encode.c"
    "libcoap/src/mem.c"
    "libcoap/src/net.c"
    "libcoap/src/option.c"
    "libcoap/src/pdu.c"
    "libcoap/src/resource.c"
    "libcoap/src/str.c"
    "libcoap/src/subscribe.c"
    "libcoap/src/uri.c")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "${include_dirs}"
                    REQUIRES lwip mbedtls)

# Silence format truncation warning, until it is fixed upstream
# set_source_files_properties(libcoap/src/coap_debug.c PROPERTIES COMPILE_FLAGS -Wno-format-truncation)

# Needed for coap headers in public builds, also.
#
# TODO: find a way to move this to a port header
target_compile_definitions(${COMPONENT_LIB} PUBLIC WITH_POSIX)
