idf_component_register(SRC_DIRS "."
                    PRIV_INCLUDE_DIRS "." "${CMAKE_CURRENT_BINARY_DIR}"
                    PRIV_INCLUDE_DIRS "../src" "../esp_supplicant/src"
                    PRIV_REQUIRES cmock esp_common test_utils wpa_supplicant mbedtls esp_wifi esp_event)
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")

idf_component_get_property(esp_supplicant_dir wpa_supplicant COMPONENT_DIR)

# Calculate MD5 value of header file esp_wifi_driver.h
file(MD5 ${esp_supplicant_dir}/esp_supplicant/src/esp_wifi_driver.h WIFI_SUPPLICANT_MD5)
string(SUBSTRING "${WIFI_SUPPLICANT_MD5}" 0 7 WIFI_SUPPLICANT_MD5)

add_definitions(-DWIFI_SUPPLICANT_MD5=\"${WIFI_SUPPLICANT_MD5}\")
add_definitions(-DCONFIG_WPA3_SAE)
add_definitions(-DCONFIG_DPP)
