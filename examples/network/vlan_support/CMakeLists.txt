# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(vlan_support)


# Enabling Vlan by injecting vlan hooks into lwip.
idf_component_get_property(lwip lwip COMPONENT_LIB)
target_compile_options(${lwip} PRIVATE "-I${PROJECT_DIR}/main")

target_compile_definitions(${lwip} PRIVATE "-DESP_IDF_LWIP_HOOK_FILENAME=\"vlan_hooks.h\""
                                            "-DETHARP_SUPPORT_VLAN=1"
                                            "-DLWIP_HOOK_VLAN_CHECK=lwip_vlan_check"
                                            "-DLWIP_HOOK_VLAN_SET=lwip_vlan_set")
