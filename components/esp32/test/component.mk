#
#Component Makefile
#

COMPONENT_EXTRA_CLEAN := test_tjpgd_logo.h

COMPONENT_ADD_LDFLAGS = -Wl,--whole-archive -l$(COMPONENT_NAME) -Wl,--no-whole-archive

COMPONENT_SRCDIRS := . test_vectors

# Calculate MD5 value of header file esp_wifi_os_adapter.h
WIFI_OS_ADAPTER_MD5_VAL=\"$(shell md5sum $(IDF_PATH)/components/esp32/include/esp_wifi_os_adapter.h | cut -c 1-7)\"
CFLAGS+=-DWIFI_OS_ADAPTER_MD5=$(WIFI_OS_ADAPTER_MD5_VAL)

# Calculate MD5 value of header file esp_wifi_crypto_types.h
WIFI_CRYPTO_MD5_VAL=\"$(shell md5sum $(IDF_PATH)/components/esp32/include/esp_wifi_crypto_types.h | cut -c 1-7)\"
CFLAGS+=-DWIFI_CRYPTO_MD5=$(WIFI_CRYPTO_MD5_VAL)

# Calculate MD5 value of header file esp_coexist_adapter.h
COEX_ADAPTER_MD5_VAL=\"$(shell md5sum $(IDF_PATH)/components/esp32/include/esp_coexist_adapter.h | cut -c 1-7)\"
CFLAGS+=-DCOEX_ADAPTER_MD5=$(COEX_ADAPTER_MD5_VAL)

test_tjpgd.o: test_tjpgd_logo.h

test_tjpgd_logo.h: $(COMPONENT_PATH)/logo.jpg
	$(summary) XXD logo.jpg
	cd $(COMPONENT_PATH); xxd -i logo.jpg $(COMPONENT_BUILD_DIR)/test_tjpgd_logo.h
