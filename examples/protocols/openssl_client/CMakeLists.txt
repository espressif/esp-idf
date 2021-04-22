# The following lines of boilerplate have to be in your project's CMakeLists
# in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

# (Not part of the boilerplate)
# This example uses an extra component for common functions such as Wi-Fi and Ethernet connection.
set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/common_components/protocol_examples_common)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(openssl_client)

if(CONFIG_EXAMPLE_OPENSSL_CLIENT_URI_FROM_STDIN)
    # This ca.crt is used when connecting to local(python executed) server
    target_add_binary_data(openssl_client.elf "server_certs/ca.crt" TEXT)
else()
    # This ca.crt is used when connecting to www.baidu.com
    target_add_binary_data(openssl_client.elf "main/baidu_ca.crt" TEXT)
endif()
