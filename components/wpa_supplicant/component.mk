# supplicant make file

COMPONENT_PRIV_INCLUDEDIRS := src src/utils esp_supplicant/src
COMPONENT_SRCDIRS := port src/ap src/common src/crypto src/eap_peer src/rsn_supp src/tls src/utils src/wps esp_supplicant/src
COMPONENT_ADD_INCLUDEDIRS := include port/include esp_supplicant/include src/utils

ifeq ($(CONFIG_ESP_WIFI_SOFTAP_SUPPORT), y)
    COMPONENT_OBJEXCLUDE += src/esp_hostap.o
endif

ifeq ($(CONFIG_WPA_MBEDTLS_CRYPTO), y)
    COMPONENT_OBJEXCLUDE += src/tls/asn1.o \
    src/tls/bignum.o \
    src/tls/pkcs1.o \
    src/tls/pkcs5.o \
    src/tls/pkcs8.o \
    src/tls/rsa.o \
    src/tls/tls_internal.o \
    src/tls/tlsv1_client.o \
    src/tls/tlsv1_client_read.o \
    src/tls/tlsv1_client_write.o \
    src/tls/tlsv1_common.o \
    src/tls/tlsv1_cred.o \
    src/tls/tlsv1_record.o \
    src/tls/tlsv1_server.o \
    src/tls/tlsv1_server_read.o \
    src/tls/tlsv1_server_write.o \
    src/tls/x509v3.o \
    src/crypto/aes-ctr.o \
    src/crypto/aes-cbc.o \
    src/crypto/aes-internal-dec.o \
    src/crypto/aes-internal-enc.o \
    src/crypto/aes-internal.o \
    src/crypto/crypto_internal-cipher.o \
    src/crypto/crypto_internal-modexp.o \
    src/crypto/crypto_internal-rsa.o \
    src/crypto/crypto_mbedtls-rsa.o \
    src/crypto/crypto_internal.o \
    src/crypto/md5-internal.o \
    src/crypto/md5.o \
    src/crypto/sha1-internal.o \
    src/crypto/sha1-pbkdf2.o \
    src/crypto/sha1.o \
    src/crypto/sha256-internal.o \
    src/crypto/sha256.o
else
    COMPONENT_OBJEXCLUDE += src/crypto/tls_mbedtls.o \
    src/crypto/crypto_mbedtls.o \
    src/crypto/crypto_mbedtls-bignum.o \
    src/crypto/crypto_mbedtls-ec.o
endif

ifneq ($(CONFIG_MBEDTLS_RC4_DISABLED), y)
    COMPONENT_OBJEXCLUDE += src/crypto/rc4.o
endif
ifeq ($(CONFIG_MBEDTLS_DES_C), y)
    COMPONENT_OBJEXCLUDE += src/crypto/des-internal.o
endif
ifeq ($(CONFIG_MBEDTLS_CMAC_C), y)
    COMPONENT_OBJEXCLUDE += src/crypto/aes-omac1.o
endif
ifeq ($(CONFIG_MBEDTLS_NIST_KW_C), y)
    COMPONENT_OBJEXCLUDE += src/crypto/aes-wrap.o
    COMPONENT_OBJEXCLUDE += src/crypto/aes-unwrap.o
    COMPONENT_OBJEXCLUDE += src/crypto/aes-ccm.o
endif

ifneq ($(CONFIG_WPA_11KV_SUPPORT), y)
    COMPONENT_OBJEXCLUDE += src/common/rrm.o \
    src/common/wnm_sta.o \
    src/common/bss.o \
    src/common/scan.o \
    src/common/ieee802_11_common.o \
    esp_supplicant/src/esp_common.o \
    esp_supplicant/src/esp_scan.o
endif
ifneq ($(CONFIG_WPA_MBO_SUPPORT), y)
    COMPONENT_OBJEXCLUDE += src/common/mbo.o
endif

CFLAGS += -DCONFIG_SHA256 -DCONFIG_DPP -DCONFIG_IEEE80211W -DESP_SUPPLICANT -DIEEE8021X_EAPOL -DEAP_PEER_METHOD -DEAP_TLS -DEAP_TTLS -DEAP_PEAP -DEAP_MSCHAPv2 -DUSE_WPA2_TASK -DCONFIG_WPS2 -DCONFIG_WPS_PIN -DUSE_WPS_TASK -DESPRESSIF_USE -DESP32_WORKAROUND -DCONFIG_ECC -DCONFIG_WNM -D__ets__ -Wno-strict-aliasing

ifdef CONFIG_ESP32_WIFI_ENABLE_WPA3_SAE
    CFLAGS += -DCONFIG_WPA3_SAE
endif
ifdef CONFIG_WPA_WPS_STRICT
	CFLAGS += -DCONFIG_WPS_STRICT
endif
ifdef CONFIG_WPA_MBO_SUPPORT
	CFLAGS += -DCONFIG_MBO
endif
ifdef CONFIG_WPA_SUITE_B_192
	CFLAGS += -DCONFIG_SUITEB192
endif
ifdef CONFIG_WPA_SUITE_B
	CFLAGS += -DCONFIG_SUITEB
endif
ifdef CONFIG_ESP_WIFI_GCMP_SUPPORT
	CFLAGS += -DCONFIG_GCMP
endif
ifdef CONFIG_ESP_WIFI_GMAC_SUPPORT
	CFLAGS += -DCONFIG_GMAC
endif
