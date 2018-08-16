set(COMPONENT_ADD_INCLUDEDIRS asio/asio/include port/include)
set(COMPONENT_SRCDIRS asio/asio/src)

set(COMPONENT_SRCEXCLUDE asio/asio/src/asio_ssl.cpp)

set(COMPONENT_REQUIRES lwip)

register_component()
