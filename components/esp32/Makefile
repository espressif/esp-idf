#
# Component Makefile
#
# This Makefile should, at the very least, just include $(SDK_PATH)/make/component.mk. By default, 
# this will take the sources in this directory, compile them and link them into 
# lib(subdirectory_name).a in the build directory. This behaviour is entirely configurable,
# please read the SDK documents if you need to do this.
#

LIBS := crypto core net80211 phy rtc pp wpa wps
                           
COMPONENT_ADD_LDFLAGS := -lesp32 \
                           $(abspath libhal.a) \
                           -L$(abspath lib) \
                           $(addprefix -l,$(LIBS)) \
                          -L $(abspath ld) \
                          -T eagle.fpga32.v7.ld \
                          -T eagle.fpga32.rom.addr.v7.ld 

include $(SDK_PATH)/make/component.mk
