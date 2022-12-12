cmake_minimum_required(VERSION 3.16)

set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/ethernet/basic/components/ethernet_init)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(eth2ap)
