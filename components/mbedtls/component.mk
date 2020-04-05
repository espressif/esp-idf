#
# Component Makefile
#

COMPONENT_ADD_INCLUDEDIRS := port/include mbedtls/include esp_crt_bundle/include

COMPONENT_SRCDIRS := mbedtls/library port port/$(IDF_TARGET) esp_crt_bundle

COMPONENT_OBJEXCLUDE := mbedtls/library/net_sockets.o

COMPONENT_SUBMODULES += mbedtls


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

