set(srcs "port/os_xtensa.c"
    "src/ap/ap_config.c"
    "src/ap/ieee802_1x.c"
    "src/ap/wpa_auth.c"
    "src/ap/wpa_auth_ie.c"
    "src/common/sae.c"
    "src/common/wpa_common.c"
    "src/utils/bitfield.c"
    "src/crypto/aes-siv.c"
    "src/crypto/sha256-kdf.c"
    "src/crypto/ccmp.c"
    "src/crypto/aes-gcm.c"
    "src/crypto/crypto_ops.c"
    "src/crypto/dh_group5.c"
    "src/crypto/dh_groups.c"
    "src/crypto/ms_funcs.c"
    "src/crypto/sha1-tlsprf.c"
    "src/crypto/sha256-tlsprf.c"
    "src/crypto/sha384-tlsprf.c"
    "src/crypto/sha256-prf.c"
    "src/crypto/sha1-prf.c"
    "src/crypto/sha384-prf.c"
    "src/crypto/md4-internal.c"
    "src/eap_peer/chap.c"
    "src/eap_peer/eap.c"
    "src/eap_peer/eap_common.c"
    "src/eap_peer/eap_mschapv2.c"
    "src/eap_peer/eap_peap.c"
    "src/eap_peer/eap_peap_common.c"
    "src/eap_peer/eap_tls.c"
    "src/eap_peer/eap_tls_common.c"
    "src/eap_peer/eap_ttls.c"
    "src/eap_peer/mschapv2.c"
    "src/eap_peer/eap_fast.c"
    "src/eap_peer/eap_fast_common.c"
    "src/eap_peer/eap_fast_pac.c"
    "src/rsn_supp/pmksa_cache.c"
    "src/rsn_supp/wpa.c"
    "src/rsn_supp/wpa_ie.c"
    "src/utils/base64.c"
    "src/utils/common.c"
    "src/utils/ext_password.c"
    "src/utils/uuid.c"
    "src/utils/wpabuf.c"
    "src/utils/wpa_debug.c"
    "src/utils/json.c"
    "src/wps/wps.c"
    "src/wps/wps_attr_build.c"
    "src/wps/wps_attr_parse.c"
    "src/wps/wps_attr_process.c"
    "src/wps/wps_common.c"
    "src/wps/wps_dev_attr.c"
    "src/wps/wps_enrollee.c"
    "src/wps/wps_registrar.c"
    "src/wps/wps_validate.c")

set(esp_srcs "esp_supplicant/src/esp_wpa2.c"
    "esp_supplicant/src/esp_wpa_main.c"
    "esp_supplicant/src/esp_wpas_glue.c"
    "esp_supplicant/src/esp_wps.c"
    "esp_supplicant/src/esp_wpa3.c")
if(CONFIG_ESP_WIFI_SOFTAP_SUPPORT)
    set(esp_srcs ${esp_srcs} "esp_supplicant/src/esp_hostap.c")
endif()

if(CONFIG_WPA_MBEDTLS_CRYPTO)
    set(tls_src "src/crypto/tls_mbedtls.c")
else()
    set(tls_src
    "src/tls/asn1.c"
    "src/tls/bignum.c"
    "src/tls/pkcs1.c"
    "src/tls/pkcs5.c"
    "src/tls/pkcs8.c"
    "src/tls/bignum.c"
    "src/tls/rsa.c"
    "src/tls/tls_internal.c"
    "src/tls/tlsv1_client.c"
    "src/tls/tlsv1_client_read.c"
    "src/tls/tlsv1_client_write.c"
    "src/tls/tlsv1_common.c"
    "src/tls/tlsv1_cred.c"
    "src/tls/tlsv1_record.c"
    "src/tls/tlsv1_server.c"
    "src/tls/tlsv1_server_read.c"
    "src/tls/tlsv1_server_write.c"
    "src/tls/x509v3.c")
endif()

if(CONFIG_WPA_MBEDTLS_CRYPTO)
    set(crypto_src
    "src/crypto/crypto_mbedtls.c"
    "src/crypto/crypto_mbedtls-bignum.c"
    "src/crypto/crypto_mbedtls-ec.c")
    # Add internal RC4 if RC4 is disabled in mbedtls
    if(CONFIG_MBEDTLS_RC4_DISABLED)
        set(crypto_src ${crypto_src} "src/crypto/rc4.c")
    endif()
    if(NOT CONFIG_MBEDTLS_DES_C)
        set(crypto_src ${crypto_src} "src/crypto/des-internal.c")
    endif()
    # Enabling this only for WiFi is probably not a good idea since MbedTLS
    # uses generic crypto init/update functions for this. That causes
    # binary size increment since all the other enabled module
    # functions will also linked in. Even after not using direct MbedTLS APIs
    # for these, these API are still faster since these all will be using
    # AES APIs which is using hardware AES blocks.
    if(NOT CONFIG_MBEDTLS_CMAC_C)
        set(crypto_src ${crypto_src} "src/crypto/aes-omac1.c")
    endif()
    if(NOT CONFIG_MBEDTLS_NIST_KW_C)
        set(crypto_src ${crypto_src}
        "src/crypto/aes-wrap.c"
        "src/crypto/aes-unwrap.c")
    endif()
    if(NOT CONFIG_MBEDTLS_NIST_KW_C OR NOT CONFIG_MBEDTLS_CMAC_C OR NOT CONFIG_MBEDTLS_CCM_C)
        set(crypto_src ${crypto_src} "src/crypto/aes-ccm.c")
    endif()
else()
    set(crypto_src
    "src/crypto/rc4.c"
    "src/crypto/aes-ctr.c"
    "src/crypto/aes-cbc.c"
    "src/crypto/aes-ccm.c"
    "src/crypto/aes-internal-dec.c"
    "src/crypto/aes-internal-enc.c"
    "src/crypto/aes-internal.c"
    "src/crypto/aes-omac1.c"
    "src/crypto/aes-unwrap.c"
    "src/crypto/aes-wrap.c"
    "src/crypto/crypto_internal-cipher.c"
    "src/crypto/crypto_internal-modexp.c"
    "src/crypto/crypto_internal-rsa.c"
    "src/crypto/crypto_mbedtls-rsa.c"
    "src/crypto/crypto_internal.c"
    "src/crypto/des-internal.c"
    "src/crypto/md4-internal.c"
    "src/crypto/md5-internal.c"
    "src/crypto/md5.c"
    "src/crypto/sha1-internal.c"
    "src/crypto/sha1-pbkdf2.c"
    "src/crypto/sha1.c"
    "src/crypto/sha1-tprf.c"
    "src/crypto/sha256-internal.c"
    "src/crypto/sha384-internal.c"
    "src/crypto/sha512-internal.c"
    "src/crypto/sha256.c")
endif()

if(CONFIG_WPA_11KV_SUPPORT)
    set(roaming_src
    "src/common/rrm.c"
    "src/common/wnm_sta.c"
    "src/common/bss.c"
    "src/common/scan.c"
    "src/common/ieee802_11_common.c"
    "esp_supplicant/src/esp_common.c"
    "esp_supplicant/src/esp_scan.c"
    )
else()
    set(roaming_src "")
endif()

if(CONFIG_WPA_MBO_SUPPORT)
    set(mbo_src "src/common/mbo.c")
else()
    set(mbo_src "")
endif()

if(CONFIG_WPA_DPP_SUPPORT)
    set(dpp_src "src/common/dpp.c"
    "esp_supplicant/src/esp_dpp.c")
else()
    set(dpp_src "")
endif()

idf_component_register(SRCS "${srcs}" ${esp_srcs} "${tls_src}" "${roaming_src}"
                            "${crypto_src}" "${mbo_src}" "${dpp_src}"
                    INCLUDE_DIRS include port/include esp_supplicant/include
                    PRIV_INCLUDE_DIRS src src/utils esp_supplicant/src
                    PRIV_REQUIRES mbedtls esp_timer)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-strict-aliasing -Wno-write-strings)
target_compile_definitions(${COMPONENT_LIB} PRIVATE
    __ets__
    ESP_SUPPLICANT
    IEEE8021X_EAPOL
    EAP_PEER_METHOD
    EAP_MSCHAPv2
    EAP_TTLS
    EAP_TLS
    EAP_PEAP
    EAP_FAST
    USE_WPA2_TASK
    CONFIG_WPS2
    CONFIG_WPS_PIN
    USE_WPS_TASK
    ESPRESSIF_USE
    ESP32_WORKAROUND
    CONFIG_ECC
    CONFIG_IEEE80211W
    CONFIG_SHA256
    CONFIG_WNM
    )

if(CONFIG_ESP32_WIFI_ENABLE_WPA3_SAE)
    target_compile_definitions(${COMPONENT_LIB} PRIVATE CONFIG_WPA3_SAE)
endif()
if(CONFIG_WPA_WPS_STRICT)
    target_compile_definitions(${COMPONENT_LIB} PRIVATE CONFIG_WPS_STRICT)
endif()
if(CONFIG_WPA_SUITE_B_192)
    target_compile_definitions(${COMPONENT_LIB} PRIVATE CONFIG_SUITEB192)
endif()
if(CONFIG_WPA_SUITE_B)
    target_compile_definitions(${COMPONENT_LIB} PRIVATE CONFIG_SUITEB)
endif()
if(CONFIG_ESP_WIFI_GCMP_SUPPORT)
    target_compile_definitions(${COMPONENT_LIB} PRIVATE CONFIG_GCMP)
endif()
if(CONFIG_ESP_WIFI_GMAC_SUPPORT)
    target_compile_definitions(${COMPONENT_LIB} PRIVATE CONFIG_GMAC)
endif()

if(CONFIG_WPA_MBO_SUPPORT)
    target_compile_definitions(${COMPONENT_LIB} PRIVATE CONFIG_MBO)
endif()
if(CONFIG_WPA_DPP_SUPPORT)
    target_compile_definitions(${COMPONENT_LIB} PRIVATE CONFIG_DPP)
endif()
set_property(TARGET ${COMPONENT_LIB} APPEND PROPERTY LINK_INTERFACE_MULTIPLICITY 3)
