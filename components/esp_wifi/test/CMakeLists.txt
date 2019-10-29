set(srcdirs ".")
if(IDF_TARGET STREQUAL "esp32")
    list(APPEND srcdirs "esp32")
endif()

idf_component_register(SRC_DIRS ${srcdirs}
                       INCLUDE_DIRS . ${CMAKE_CURRENT_BINARY_DIR}
                       REQUIRES unity test_utils nvs_flash ulp esp_common
                      )

idf_component_get_property(esp_wifi_dir esp_wifi COMPONENT_DIR)

# Calculate MD5 value of header file esp_wifi_os_adapter.h
file(MD5 ${esp_wifi_dir}/include/esp_private/wifi_os_adapter.h WIFI_OS_ADAPTER_MD5)
string(SUBSTRING "${WIFI_OS_ADAPTER_MD5}" 0 7 WIFI_OS_ADAPTER_MD5)

# Calculate MD5 value of header file esp_wifi_crypto_types.h
file(MD5 ${esp_wifi_dir}/include/esp_wifi_crypto_types.h WIFI_CRYPTO_MD5)
string(SUBSTRING "${WIFI_CRYPTO_MD5}" 0 7 WIFI_CRYPTO_MD5)

# Calculate MD5 value of header file esp_coexist_adapter.h
file(MD5 ${esp_wifi_dir}/include/esp_coexist_adapter.h COEX_ADAPTER_MD5)
string(SUBSTRING "${COEX_ADAPTER_MD5}" 0 7 COEX_ADAPTER_MD5)

# Calculate MD5 value of header file esp_wifi_types.h
file(MD5 ${esp_wifi_dir}/include/esp_wifi_types.h WIFI_TYPE_MD5)
string(SUBSTRING "${WIFI_TYPE_MD5}" 0 7 WIFI_TYPE_MD5)

# Calculate MD5 value of header file esp_wifi.h
file(MD5 ${esp_wifi_dir}/include/esp_wifi.h WIFI_ESP_WIFI_MD5)
string(SUBSTRING "${WIFI_ESP_WIFI_MD5}" 0 7 WIFI_ESP_WIFI_MD5)

add_definitions(-DWIFI_OS_ADAPTER_MD5=\"${WIFI_OS_ADAPTER_MD5}\")
add_definitions(-DWIFI_CRYPTO_MD5=\"${WIFI_CRYPTO_MD5}\")
add_definitions(-DCOEX_ADAPTER_MD5=\"${COEX_ADAPTER_MD5}\")
add_definitions(-DWIFI_TYPE_MD5=\"${WIFI_TYPE_MD5}\")
add_definitions(-DWIFI_ESP_WIFI_MD5=\"${WIFI_ESP_WIFI_MD5}\")
