idf_component_register(SRCS
                        "test_crypto.c"
                        "test_dpp.c"
                        "test_eloop.c"
                        "test_fast_pbkdf2.c"
                        "test_offchannel.c"
                        "test_sae.c"
                        "test_wpa_supplicant_main.c"
                    PRIV_INCLUDE_DIRS "."
                    PRIV_REQUIRES wpa_supplicant mbedtls esp_wifi esp_event unity
                    WHOLE_ARCHIVE)

idf_component_get_property(esp_supplicant_dir wpa_supplicant COMPONENT_DIR)

# Calculate MD5 value of header file esp_wifi_driver.h
file(MD5 ${esp_supplicant_dir}/esp_supplicant/src/esp_wifi_driver.h WIFI_SUPPLICANT_MD5)
string(SUBSTRING "${WIFI_SUPPLICANT_MD5}" 0 7 WIFI_SUPPLICANT_MD5)

# Steal some private include directories from wpa_supplicant
target_include_directories(${COMPONENT_LIB} PRIVATE ${esp_supplicant_dir}/esp_supplicant/src)
target_include_directories(${COMPONENT_LIB} PRIVATE ${esp_supplicant_dir}/src)

add_definitions(-DWIFI_SUPPLICANT_MD5=\"${WIFI_SUPPLICANT_MD5}\")
add_definitions(-DCONFIG_WPA3_SAE)
add_definitions(-DCONFIG_DPP)
