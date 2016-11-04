COMPONENT_ADD_INCLUDEDIRS := include
COMPONENT_PRIV_INCLUDEDIRS := include_priv

# include configuration macros early
include $(IDF_PATH)/make/common.mk

ifdef IS_BOOTLOADER_BUILD
# share "private" headers with the bootloader component
# eventual goal: all functionality that needs this lives in bootloader_support
COMPONENT_ADD_INCLUDEDIRS += include_priv
endif

COMPONENT_SRCDIRS := src

#
# Secure boot signing key support
#
ifdef CONFIG_SECURE_BOOTLOADER_ENABLED

# this path is created relative to the component build directory
SECURE_BOOT_VERIFICATION_KEY := $(abspath signature_verification_key.bin)

$(SECURE_BOOT_SIGNING_KEY):
	@echo "Need to generate secure boot signing key."
	@echo "One way is to run this command:"
	@echo "$(ESPSECUREPY) generate_signing_key $@"
	@echo "Keep key file safe after generating."
	@echo "(See secure boot documentation for risks & alternatives.)"
	@exit 1

$(SECURE_BOOT_VERIFICATION_KEY): $(SECURE_BOOT_SIGNING_KEY)
	$(ESPSECUREPY) extract_public_key --keyfile $< $@

COMPONENT_EXTRA_CLEAN += $(SECURE_BOOT_VERIFICATION_KEY)

COMPONENT_EMBED_FILES := $(SECURE_BOOT_VERIFICATION_KEY)

endif

include $(IDF_PATH)/make/component_common.mk
