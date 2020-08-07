# The following lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.5)

# This example uses an extra component for common functions for Bluetooth HCI layer.
set(EXTRA_COMPONENT_DIRS $ENV{IDF_PATH}/examples/bluetooth/hci/hci_common_component)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(ble_adv_scan)
