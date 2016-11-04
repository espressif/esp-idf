#
# Main Makefile. This is basically the same as a component makefile.
#
# This Makefile should, at the very least, just include $(SDK_PATH)/make/component_common.mk. By default, 
# this will take the sources in the src/ directory, compile them and link them into 
# lib(subdirectory_name).a in the build directory. This behaviour is entirely configurable,
# please read the ESP-IDF documents if you need to do this.
#

COMPONENT_ADD_INCLUDEDIRS :=	\
				udrv/include				\
				btif/include				\
				audio_a2dp_hw/include			\
				embdrv/sbc/encoder/include		\
				embdrv/sbc/decoder/include		\
				include					\

COMPONENT_SRCDIRS :=	\
			app_core				\
			app_project				\
			udrv/ulinux				\
			embdrv/sbc/encoder			\
			embdrv/sbc/decoder			\
			btif/co					\
			btif

CFLAGS += -Wno-error=unused-label -Wno-error=return-type -Wno-error=missing-braces -Wno-error=pointer-sign -Wno-error=parentheses -I./include


include $(IDF_PATH)/make/component_common.mk
