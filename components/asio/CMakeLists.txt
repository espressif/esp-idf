set(COMPONENT_ADD_INCLUDEDIRS asio/asio/include port/include)
set(COMPONENT_SRCS "asio/asio/src/asio.cpp")

set(COMPONENT_REQUIRES lwip)

register_component()
