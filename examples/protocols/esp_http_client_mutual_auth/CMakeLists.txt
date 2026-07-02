cmake_minimum_required(VERSION 3.22)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
idf_build_set_property(MINIMAL_BUILD ON)

project(esp_http_client_mutual_auth)

if(CONFIG_EXAMPLE_MUTUAL_AUTH_USE_HW_RSA_DS)
    # Flash the pre-built esp_secure_cert partition (device cert + DS context) so it
    # is included in the QEMU flash image created by esptool merge_bin.
    esptool_py_flash_to_partition(
        flash esp_secure_cert
        "${CMAKE_CURRENT_SOURCE_DIR}/main/certs/esp_secure_cert_data/esp_secure_cert.bin"
    )
endif()
