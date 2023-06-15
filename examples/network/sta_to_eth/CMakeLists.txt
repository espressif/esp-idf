# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

# This example needs a DNS server: let's use the simple DNS server implementation from captive portal example
set(EXTRA_COMPONENT_DIRS    $ENV{IDF_PATH}/examples/protocols/http_server/captive_portal/components/dns_server
                            $ENV{IDF_PATH}/examples/ethernet/basic/components/ethernet_init)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(sta_to_eth)
