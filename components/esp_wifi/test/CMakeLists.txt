set(COMPONENT_SRCDIRS ".")
set(COMPONENT_ADD_INCLUDEDIRS ". ${CMAKE_CURRENT_BINARY_DIR}")

set(COMPONENT_REQUIRES unity test_utils nvs_flash ulp esp_common)

register_component()

idf_build_get_property(idf_path IDF_PATH)
# Calculate MD5 value of header file esp_wifi_os_adapter.h
execute_process(COMMAND md5sum ${idf_path}/components/esp_wifi/include/esp_wifi_os_adapter.h
                COMMAND cut -c 1-7
                OUTPUT_VARIABLE WIFI_OS_ADAPTER_MD5
                OUTPUT_STRIP_TRAILING_WHITESPACE)

# Calculate MD5 value of header file esp_wifi_crypto_types.h
execute_process(COMMAND md5sum ${idf_path}/components/esp_wifi/include/esp_wifi_crypto_types.h
                COMMAND cut -c 1-7
                OUTPUT_VARIABLE WIFI_CRYPTO_MD5
                OUTPUT_STRIP_TRAILING_WHITESPACE)

# Calculate MD5 value of header file esp_coexist_adapter.h
execute_process(COMMAND md5sum ${idf_path}/components/esp_wifi/include/esp_coexist_adapter.h
                COMMAND cut -c 1-7
                OUTPUT_VARIABLE COEX_ADAPTER_MD5
                OUTPUT_STRIP_TRAILING_WHITESPACE)

add_definitions(-DWIFI_OS_ADAPTER_MD5=\"${WIFI_OS_ADAPTER_MD5}\")
add_definitions(-DWIFI_CRYPTO_MD5=\"${WIFI_CRYPTO_MD5}\")
add_definitions(-DCOEX_ADAPTER_MD5=\"${COEX_ADAPTER_MD5}\")
