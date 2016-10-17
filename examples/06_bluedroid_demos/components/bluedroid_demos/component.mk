#
# Main Makefile. This is basically the same as a component makefile.
#
# This Makefile should, at the very least, just include $(SDK_PATH)/make/component_common.mk. By default, 
# this will take the sources in the src/ directory, compile them and link them into 
# lib(subdirectory_name).a in the build directory. This behaviour is entirely configurable,
# please read the ESP-IDF documents if you need to do this.
#

COMPONENT_SRCDIRS :=	\
			app_core				\
			app_client_profiles/battery_c		\
			app_client_profiles			\
			app_profiles/app_sample_button		\
			app_profiles/app_WX_airsync		\
			app_profiles				\
			app_project				
	
CFLAGS += -Wno-error=unused-label -Wno-error=return-type -Wno-error=missing-braces -Wno-error=pointer-sign -Wno-error=parentheses -I./include


include $(IDF_PATH)/make/component_common.mk
