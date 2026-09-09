include($ENV{IDF_PATH}/tools/cmake/version.cmake)

set(public_requires "freertos")

if("${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}" VERSION_GREATER_EQUAL "5.3")
    list(APPEND public_requires "esp_driver_gpio")
else()
    list(APPEND public_requires "driver")
endif()

idf_component_register(SRC_DIRS "src"
                       INCLUDE_DIRS "include"
                       REQUIRES ${public_requires})
