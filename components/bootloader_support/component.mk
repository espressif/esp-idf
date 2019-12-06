COMPONENT_ADD_INCLUDEDIRS := include

ifdef IS_BOOTLOADER_BUILD
# share "include_bootloader" headers with bootloader main component
COMPONENT_ADD_INCLUDEDIRS += include_bootloader
else
COMPONENT_PRIV_INCLUDEDIRS := include_bootloader
endif

COMPONENT_SRCDIRS := src

ifndef IS_BOOTLOADER_BUILD
COMPONENT_SRCDIRS += src/idf  # idf sub-directory contains platform agnostic IDF versions
else
COMPONENT_SRCDIRS += src/$(IDF_TARGET)  # one sub-dir per chip
endif

ifndef IS_BOOTLOADER_BUILD
COMPONENT_OBJEXCLUDE := src/bootloader_init.o
endif

COMPONENT_OBJEXCLUDE += src/bootloader_flash_config_esp32s2beta.o \
			src/bootloader_efuse_esp32s2beta.o

#
# Secure boot signing key support
#
ifdef CONFIG_SECURE_SIGNED_APPS

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
	@echo "docs/security/secure-boot.rst for details."
	exit 1

# copy it into the build dir, so the secure boot verification key has
# a predictable file name
$(SECURE_BOOT_VERIFICATION_KEY): $(ORIG_SECURE_BOOT_VERIFICATION_KEY) $(SDKCONFIG_MAKEFILE)
	$(summary) CP $< $@
	cp $< $@
endif #CONFIG_SECURE_BOOT_BUILD_SIGNED_BINARIES

COMPONENT_EXTRA_CLEAN += $(SECURE_BOOT_VERIFICATION_KEY)

COMPONENT_EMBED_FILES := $(SECURE_BOOT_VERIFICATION_KEY)

endif #CONFIG_SECURE_SIGNED_APPS
