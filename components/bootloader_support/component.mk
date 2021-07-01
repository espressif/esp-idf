COMPONENT_ADD_INCLUDEDIRS := include

ifdef IS_BOOTLOADER_BUILD
# share "include_bootloader" headers with bootloader main component
COMPONENT_ADD_INCLUDEDIRS += include_bootloader
else
COMPONENT_PRIV_INCLUDEDIRS := include_bootloader
endif

COMPONENT_SRCDIRS := src \
			src/secure_boot_v2 \
			src/secure_boot_v1

ifndef IS_BOOTLOADER_BUILD
COMPONENT_SRCDIRS += src/idf  # idf sub-directory contains platform agnostic IDF versions
else
COMPONENT_SRCDIRS += src/$(IDF_TARGET)  # one sub-dir per chip
ifdef CONFIG_SECURE_FLASH_ENC_ENABLED
COMPONENT_SRCDIRS += src/flash_encryption
endif
endif

ifndef IS_BOOTLOADER_BUILD
COMPONENT_OBJEXCLUDE := src/bootloader_init.o \
			src/bootloader_panic.o \
			src/bootloader_clock_loader.o \
			src/bootloader_console.o \
			src/bootloader_console_loader.o
endif

COMPONENT_OBJEXCLUDE += src/bootloader_flash_config_esp32s2.o \
			src/bootloader_flash_config_esp32s3.o \
			src/bootloader_flash_config_esp32c3.o \
			src/bootloader_flash_config_esp32h2.o \
			src/bootloader_efuse_esp32s2.o \
			src/bootloader_efuse_esp32s3.o \
			src/bootloader_efuse_esp32c3.o \
			src/bootloader_efuse_esp32h2.o \
			src/bootloader_random_esp32s2.o \
			src/bootloader_random_esp32s3.o \
			src/bootloader_random_esp32c3.o \
			src/bootloader_random_esp32h2.o

ifdef IS_BOOTLOADER_BUILD
	ifndef CONFIG_SECURE_SIGNED_APPS_ECDSA_SCHEME
		COMPONENT_OBJEXCLUDE += src/secure_boot_v1/secure_boot_signatures_bootloader.o
	endif

	ifndef CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME
		COMPONENT_OBJEXCLUDE += src/secure_boot_v2/secure_boot_signatures_bootloader.o
	endif

	ifndef CONFIG_SECURE_BOOT_V1_ENABLED
		COMPONENT_OBJEXCLUDE += src/secure_boot_v1/secure_boot.o
	endif

	ifndef CONFIG_SECURE_BOOT_V2_ENABLED
		COMPONENT_OBJEXCLUDE += src/secure_boot_v2/secure_boot.o
	endif

	ifndef CONFIG_SECURE_BOOT
		COMPONENT_OBJEXCLUDE += src/${IDF_TARGET}/secure_boot_secure_features.o
	endif

	ifndef CONFIG_SECURE_FLASH_ENC_ENABLED
		COMPONENT_OBJEXCLUDE += src/${IDF_TARGET}/flash_encryption_secure_features.o
	endif

	COMPONENT_OBJEXCLUDE += src/secure_boot_v1/secure_boot_signatures_app.o \
				src/secure_boot_v2/secure_boot_signatures_app.o
else
	ifndef CONFIG_SECURE_SIGNED_APPS_ECDSA_SCHEME
		COMPONENT_OBJEXCLUDE += src/secure_boot_v1/secure_boot_signatures_app.o
	endif

	ifndef CONFIG_SECURE_SIGNED_APPS_RSA_SCHEME
		COMPONENT_OBJEXCLUDE += src/secure_boot_v2/secure_boot_signatures_app.o
	endif

	COMPONENT_OBJEXCLUDE += src/secure_boot_v1/secure_boot_signatures_bootloader.o \
				src/secure_boot_v1/secure_boot.o \
				src/secure_boot_v2/secure_boot_signatures_bootloader.o \
				src/secure_boot_v2/secure_boot.o
endif # IS_BOOTLOADER_BUILD

#
# Secure boot signing key support
#
ifdef CONFIG_SECURE_SIGNED_APPS

ifdef CONFIG_SECURE_SIGNED_APPS_ECDSA_SCHEME
# this path is created relative to the component build directory
SECURE_BOOT_VERIFICATION_KEY := $(abspath signature_verification_key.bin)

ifdef CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES
# verification key derived from signing key.
$(SECURE_BOOT_VERIFICATION_KEY): $(SECURE_BOOT_SIGNING_KEY) $(SDKCONFIG_MAKEFILE)
	$(ESPSECUREPY) extract_public_key --keyfile $< $@
else
# find the configured public key file
ORIG_SECURE_BOOT_VERIFICATION_KEY := $(call resolvepath,$(call dequote,$(CONFIG_SECURE_BOOT_VERIFICATION_KEY)),$(PROJECT_PATH))

$(ORIG_SECURE_BOOT_VERIFICATION_KEY):
	@echo "Secure boot verification public key '$@' missing."
	@echo "This can be extracted from the private signing key, see"
	@echo "docs/security/secure-boot-v1.rst for details."
	exit 1

# copy it into the build dir, so the secure boot verification key has
# a predictable file name
$(SECURE_BOOT_VERIFICATION_KEY): $(ORIG_SECURE_BOOT_VERIFICATION_KEY) $(SDKCONFIG_MAKEFILE)
	$(summary) CP $< $@
	cp $< $@
endif #CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES

COMPONENT_EXTRA_CLEAN += $(SECURE_BOOT_VERIFICATION_KEY)

COMPONENT_EMBED_FILES := $(SECURE_BOOT_VERIFICATION_KEY)

endif #CONFIG_SECURE_SIGNED_APPS_ECDSA_SCHEME
endif #CONFIG_SECURE_SIGNED_APPS
