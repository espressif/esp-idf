set(COMPONENT_ADD_INCLUDEDIRS port/include nghttp2/lib/includes)
set(COMPONENT_SRCDIRS nghttp2/lib port)

set(COMPONENT_REQUIRES lwip mbedtls)

register_component()
