#
#Component Makefile
#

COMPONENT_PRIV_INCLUDEDIRS := ../src
COMPONENT_SRCDIRS := .

COMPONENT_ADD_LDFLAGS = -Wl,--whole-archive -l$(COMPONENT_NAME) -Wl,--no-whole-archive

WIFI_SUPPLICANT_MD5_VAL=\"$(shell md5sum $(IDF_PATH)/components/wpa_supplicant/src/esp_supplicant/esp_wifi_driver.h | cut -c 1-7)\"
CFLAGS+=-DWIFI_SUPPLICANT_MD5=$(WIFI_SUPPLICANT_MD5_VAL)
