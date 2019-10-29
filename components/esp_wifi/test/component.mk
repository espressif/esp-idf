#
#Component Makefile
#

COMPONENT_ADD_LDFLAGS = -Wl,--whole-archive -l$(COMPONENT_NAME) -Wl,--no-whole-archive

COMPONENT_SRCDIRS := .
COMPONENT_SRCDIRS += esp32

# Calculate MD5 value of header file esp_wifi_os_adapter.h
WIFI_OS_ADAPTER_MD5_VAL=\"$(shell md5sum $(IDF_PATH)/components/esp_wifi/include/esp_private/wifi_os_adapter.h | cut -c 1-7)\"
CFLAGS+=-DWIFI_OS_ADAPTER_MD5=$(WIFI_OS_ADAPTER_MD5_VAL)

# Calculate MD5 value of header file esp_wifi_crypto_types.h
WIFI_CRYPTO_MD5_VAL=\"$(shell md5sum $(IDF_PATH)/components/esp_wifi/include/esp_wifi_crypto_types.h | cut -c 1-7)\"
CFLAGS+=-DWIFI_CRYPTO_MD5=$(WIFI_CRYPTO_MD5_VAL)

# Calculate MD5 value of header file esp_coexist_adapter.h
COEX_ADAPTER_MD5_VAL=\"$(shell md5sum $(IDF_PATH)/components/esp_wifi/include/esp_coexist_adapter.h | cut -c 1-7)\"
CFLAGS+=-DCOEX_ADAPTER_MD5=$(COEX_ADAPTER_MD5_VAL)

# Calculate MD5 value of header file esp_wifi_types.h
WIFI_TYPE_MD5_VAL=\"$(shell md5sum $(IDF_PATH)/components/esp_wifi/include/esp_wifi_types.h | cut -c 1-7)\"
CFLAGS+=-DWIFI_TYPE_MD5=$(WIFI_TYPE_MD5_VAL)

# Calculate MD5 value of header file esp_wifi.h
WIFI_ESP_WIFI_MD5_VAL=\"$(shell md5sum $(IDF_PATH)/components/esp_wifi/include/esp_wifi.h | cut -c 1-7)\"
CFLAGS+=-DWIFI_ESP_WIFI_MD5=$(WIFI_ESP_WIFI_MD5_VAL)
