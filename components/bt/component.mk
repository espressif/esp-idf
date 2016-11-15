#
# Component Makefile
#

COMPONENT_ADD_INCLUDEDIRS :=	bluedroid/bta/include			\
				bluedroid/bta/sys/include		\
				bluedroid/btcore/include		\
				bluedroid/device/include		\
				bluedroid/gki/include			\
				bluedroid/hci/include			\
				bluedroid/osi/include			\
				bluedroid/utils/include			\
				bluedroid/profiles/core/include			\
				bluedroid/profiles/esp/blufi/include		\
				bluedroid/profiles/esp/include		\
				bluedroid/profiles/std/battery/include	\
				bluedroid/profiles/std/dis/include	\
				bluedroid/profiles/std/hid/include	\
				bluedroid/profiles/std/rfcomm/include	\
				bluedroid/profiles/std/include		\
				bluedroid/stack/btm/include		\
				bluedroid/stack/btu/include		\
				bluedroid/stack/gap/include		\
				bluedroid/stack/gatt/include		\
				bluedroid/stack/hcic/include		\
				bluedroid/stack/l2cap/include		\
				bluedroid/stack/sdp/include		\
				bluedroid/stack/smp/include		\
				bluedroid/stack/avct/include		\
				bluedroid/stack/avrc/include		\
				bluedroid/stack/avdt/include		\
				bluedroid/stack/a2dp/include		\
				bluedroid/stack/include			\
				bluedroid/utils/include			\
				bluedroid/api/include		\
				bluedroid/include			\
				include	

CFLAGS += -Wno-error=unused-label -Wno-error=return-type -Wno-error=missing-braces -Wno-error=pointer-sign -Wno-error=parentheses

LIBS := btdm_app coexist

COMPONENT_ADD_LDFLAGS := -lbt -L$(abspath lib) \
                           $(addprefix -l,$(LIBS)) \
                          $(LINKER_SCRIPTS)

COMPONENT_SRCDIRS := 	bluedroid/bta/dm			\
			bluedroid/bta/gatt			\
			bluedroid/bta/hh			\
			bluedroid/bta/sdp			\
			bluedroid/bta/av			\
			bluedroid/bta/ar			\
			bluedroid/bta/sys			\
			bluedroid/bta				\
			bluedroid/btcore			\
			bluedroid/btif				\
			bluedroid/device			\
			bluedroid/gki				\
			bluedroid/hci				\
			bluedroid/main				\
			bluedroid/osi				\
			bluedroid/profiles/core		\
			bluedroid/profiles/esp/blufi	\
			bluedroid/profiles/esp/ble_button	\
			bluedroid/profiles/esp/wechat_AirSync	\
			bluedroid/profiles/esp			\
			bluedroid/profiles/std/battery		\
			bluedroid/profiles/std/dis		\
			bluedroid/profiles/std/hid		\
			bluedroid/profiles/std/hid_le		\
			bluedroid/profiles/std/rfcomm		\
			bluedroid/profiles/std/sdp		\
			bluedroid/profiles/std/gap		\
			bluedroid/profiles/std			\
			bluedroid/profiles			\
			bluedroid/stack/btm			\
			bluedroid/stack/btu			\
			bluedroid/stack/gap			\
			bluedroid/stack/gatt			\
			bluedroid/stack/hcic			\
			bluedroid/stack/include			\
			bluedroid/stack/l2cap			\
			bluedroid/stack/sdp			\
			bluedroid/stack/smp			\
			bluedroid/stack/avct			\
			bluedroid/stack/avrc			\
			bluedroid/stack/avdt			\
			bluedroid/stack/a2dp			\
			bluedroid/stack				\
			bluedroid/utils				\
			bluedroid/api			\
			bluedroid				\
			.

include $(IDF_PATH)/make/component_common.mk

ALL_LIB_FILES := $(patsubst %,$(COMPONENT_PATH)/lib/lib%.a,$(LIBS))
$(COMPONENT_LIBRARY): $(ALL_LIB_FILES)

# automatically trigger a git submodule update if BT library is missing
$(eval $(call SubmoduleCheck,$(ALL_LIB_FILES),$(COMPONENT_PATH)/lib))
