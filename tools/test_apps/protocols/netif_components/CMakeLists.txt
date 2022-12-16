# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

set(driver_components esp_eth esp_wifi openthread)

# General rules for all these tests
#   * No optional dependencies
#   * Forbidden dependencies are only driver components
set(forbidden_deps ${driver_components})
set(optional_deps)
# Check manually if the ESP_NETIF is configured in sdkconfig
# Note that we cannot use Kconfig values at this stage, and therefore we have to parse
# the sdkconfig manually for the relevant "config" (CONFIG_TESTAPP_COMPONENT_...),
# in case the sdkconfig doesn't exist (clean build), we choose the default.
message(STATUS "Checking if sdkconfig contains CONFIG_TESTAPP_COMPONENT related config:")
set(config_file "${CMAKE_CURRENT_SOURCE_DIR}/sdkconfig")
if(EXISTS ${config_file})
    # If the config file exists, check for the non-default settings - LWIP or ESP-NETIF-without-LWIP
    # otherwise (file missing or defalut settings) go with ESP_NETIF
    file(READ ${config_file} config_file_content)
    string(FIND "${config_file_content}" "CONFIG_TESTAPP_COMPONENT_LWIP=y" match_lwip)
    string(FIND "${config_file_content}" "CONFIG_TESTAPP_COMPONENT_ESP_NETIF_WITHOUT_LWIP=y" match_netif_no_lwip)
    if(NOT ${match_lwip} EQUAL -1)
        set(CHECK_DEPS_FOR_LWIP 1)
    elseif(NOT ${match_netif_no_lwip} EQUAL -1)
        set(CHECK_DEPS_FOR_ESP_NETIF_WITHOUT_LWIP 1)
    endif()
endif()

if(CHECK_DEPS_FOR)
    message(STATUS "CONFIG_TESTAPP_COMPONENT_LWIP")
    set(component_under_test lwip)
    set(expected_build_components lwip)
elseif(CHECK_DEPS_FOR_ESP_NETIF_WITHOUT_LWIP)
    message(STATUS "CONFIG_TESTAPP_COMPONENT_ESP_NETIF_WITHOUT_LWIP")
    set(component_under_test esp_netif)
    set(expected_build_components esp_netif)
    list(APPEND optional_deps mbedtls)
    list(APPEND forbidden_deps lwip)    # lwip mustn't be pulled in, in "esp-netif without lwip" setup
    list(APPEND EXTRA_COMPONENT_DIRS "esp_netif_stack")
else()
    message(STATUS "CONFIG_TESTAPP_COMPONENT_ESP_NETIF")
    set(component_under_test esp_netif)
    set(expected_build_components esp_netif lwip)
endif()

set(COMPONENTS ${component_under_test} main ${optional_deps})

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

idf_build_set_property(__BUILD_COMPONENT_DEPGRAPH_ENABLED 1)

project(network_components)

# Get the actual build commponents included in the build
idf_build_get_property(build_components BUILD_COMPONENTS)

message(STATUS "Build components needed my main and ${component_under_test}:")
foreach(comp ${build_components})
    message(STATUS "${comp}")
endforeach()

# Check for all the components, these shall not be included
foreach(comp ${forbidden_deps})
    if(${comp} IN_LIST build_components)
        message(FATAL_ERROR "Component ${comp} shall not be included when building only ${component_under_test}")
    else()
        message(STATUS "-> OK: ${comp} is not included when building only ${component_under_test}")
    endif()
endforeach()

# Check for all needed components, these must be included
foreach(comp ${expected_build_components})
    if(${comp} IN_LIST build_components)
        message(STATUS "-> OK: ${comp} is pulled by ${component_under_test}")
    else()
        message(FATAL_ERROR "${comp} is expected by adding ${component_under_test}")
    endif()
endforeach()
