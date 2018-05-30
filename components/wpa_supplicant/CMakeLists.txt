set(COMPONENT_SRCDIRS src/crypto port src/fast_crypto)
set(COMPONENT_ADD_INCLUDEDIRS include port/include)

set(COMPONENT_REQUIRES "")
set(COMPONENT_PRIV_REQUIRES lwip mbedtls)

register_component()

component_compile_options(-Wno-strict-aliasing)
component_compile_definitions(
    __ets__
    EMBEDDED_SUPP
    IEEE8021X_EAPOL
    EAP_PEER_METHOD
    EAP_MSCHAPv2
    EAP_TTLS
    EAP_TLS
    EAP_PEAP
    USE_WPA2_TASK
    CONFIG_WPS2
    CONFIG_WPS_PIN
    USE_WPS_TASK
    ESPRESSIF_USE
    ESP32_WORKAROUND
    )
