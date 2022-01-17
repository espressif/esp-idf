#
# Component Makefile
#


COMPONENT_ADD_INCLUDEDIRS := port/include mbedtls/include esp_crt_bundle/include

COMPONENT_PRIV_INCLUDEDIRS := mbedtls/library

COMPONENT_SRCDIRS := mbedtls/library port port/$(IDF_TARGET) port/sha port/sha/parallel_engine port/aes port/aes/block port/md esp_crt_bundle

COMPONENT_OBJEXCLUDE := mbedtls/library/net_sockets.o

ifdef CONFIG_MBEDTLS_SSL_PROTO_GMTSSL1_1
COMPONENT_OBJEXCLUDE += \
mbedtls/library/ssl_ciphersuites.o \
mbedtls/library/ecp.o \
mbedtls/library/cipher_wrap.o \
mbedtls/library/oid.o \
mbedtls/library/ecp_curves.o \
mbedtls/library/pk_wrap.o \
mbedtls/library/ecdsa.o \
mbedtls/library/x509_crt.o \
mbedtls/library/ssl_tls.o \
mbedtls/library/ssl_cli.o
endif

COMPONENT_SUBMODULES += mbedtls


# Note: some mbedTLS hardware acceleration can be enabled/disabled by config.
#
# We don't need to exclude aes.o as these functions use a different prefix (esp_aes_x) and the
# config option only changes the prefixes in the header so mbedtls_aes_x compiles to esp_aes_x
#
# The other port-specific files don't override internal mbedTLS functions, they just add new functions.

ifndef CONFIG_MBEDTLS_HARDWARE_MPI
    COMPONENT_OBJEXCLUDE += port/esp_bignum.o port/$(IDF_TARGET)/bignum.o
endif



ifndef CONFIG_MBEDTLS_HARDWARE_SHA
    COMPONENT_OBJEXCLUDE += port/parallel_engine/esp_sha1.o port/parallel_engine/esp_sha256.o port/parallel_engine/esp_sha512.o
endif

ifdef CONFIG_MBEDTLS_CERTIFICATE_BUNDLE

GEN_CRT_BUNDLEPY := $(PYTHON) $(COMPONENT_PATH)/esp_crt_bundle/gen_crt_bundle.py
DEFAULT_CRT_DIR := ${COMPONENT_PATH}/esp_crt_bundle
X509_CERTIFICATE_BUNDLE := $(abspath x509_crt_bundle)
CUSTOM_BUNDLE_PATH := $(PROJECT_PATH)/$(CONFIG_MBEDTLS_CUSTOM_CERTIFICATE_BUNDLE_PATH)

ifdef CONFIG_MBEDTLS_CUSTOM_CERTIFICATE_BUNDLE
CRT_PATHS += $(CUSTOM_BUNDLE_PATH)
endif

ifdef CONFIG_MBEDTLS_CERTIFICATE_BUNDLE_DEFAULT_FULL
CRT_PATHS += ${DEFAULT_CRT_DIR}/cacrt_all.pem
endif

ifdef CONFIG_MBEDTLS_CERTIFICATE_BUNDLE_DEFAULT_CMN
CRT_PATHS += ${DEFAULT_CRT_DIR}/cacrt_all.pem
ARGS += --filter ${DEFAULT_CRT_DIR}/cmn_crt_authorities.csv
endif

ARGS += --input $(CRT_PATHS) -q

# Generate certificate bundle using generate_cert_bundle.py
$(X509_CERTIFICATE_BUNDLE) : $(SDKCONFIG_MAKEFILE)
	$(GEN_CRT_BUNDLEPY) $(ARGS)

COMPONENT_EXTRA_CLEAN += $(X509_CERTIFICATE_BUNDLE)

COMPONENT_EMBED_FILES := $(X509_CERTIFICATE_BUNDLE)

endif

ifdef CONFIG_MBEDTLS_DYNAMIC_BUFFER
WRAP_FUNCTIONS = mbedtls_ssl_handshake_client_step \
                 mbedtls_ssl_handshake_server_step \
                 mbedtls_ssl_read \
                 mbedtls_ssl_write \
                 mbedtls_ssl_session_reset \
                 mbedtls_ssl_free \
                 mbedtls_ssl_setup \
                 mbedtls_ssl_send_alert_message \
                 mbedtls_ssl_close_notify

COMPONENT_SRCDIRS += port/dynamic
endif

ifneq ($(origin WRAP_FUNCTIONS),undefined)
WRAP_ARGUMENT := -Wl,--wrap=
COMPONENT_ADD_LDFLAGS = -l$(COMPONENT_NAME) $(addprefix $(WRAP_ARGUMENT),$(WRAP_FUNCTIONS))
endif
