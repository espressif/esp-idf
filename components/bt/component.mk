#
# Component Makefile
#

COMPONENT_ADD_INCLUDEDIRS := ./include bluedroid/include bluedroid/osi/include bluedroid/gki/include bluedroid/btcore/include bluedroid/hci/include bluedroid/device/include bluedroid/stack/include bluedroid/app/include bluedroid/profiles/include bluedroid/bta/include bluedroid/stack/avrc/include bluedroid/stack/rfcomm/include bluedroid/bta/sys/include

CFLAGS += -Wno-error=unused-label -Wno-error=return-type -Wno-error=missing-braces -Wno-error=pointer-sign -Wno-error=parentheses

LIBS := btdm_app

COMPONENT_ADD_LDFLAGS := -lbt -L$(abspath lib) \
                           $(addprefix -l,$(LIBS)) \
                          $(LINKER_SCRIPTS)


ALL_LIB_FILES := $(patsubst %,$(COMPONENT_PATH)/lib/lib%.a,$(LIBS))
$(COMPONENT_LIBRARY): $(ALL_LIB_FILES)

COMPONENT_SRCDIRS := 	\
		bluedroid/main	\
		bluedroid/profiles/hid_le	\
		bluedroid/profiles/baterry	\
		bluedroid/profiles/sample_button	\
		bluedroid/profiles/diss	\
		bluedroid/profiles	\
		bluedroid/hci	\
		bluedroid/stack/btu	\
		bluedroid/stack/btm	\
		bluedroid/stack/gatt	\
		bluedroid/stack/avrc	\
		bluedroid/stack/hcic	\
		bluedroid/stack/srvc	\
		bluedroid/stack/gap	\
		bluedroid/stack/l2cap	\
		bluedroid/stack/smp	\
		bluedroid/stack/sdp	\
		bluedroid/stack/rfcomm	\
		bluedroid/stack	\
		bluedroid/btcore	\
		bluedroid/device	\
		bluedroid/app/app_client_profiles	\
		bluedroid/app/app_client_profiles/battery_c	\
		bluedroid/app/app_project	\
		bluedroid/app/app_profiles	\
		bluedroid/app/app_profiles/app_sample_button	\
		bluedroid/app/app_core	\
		bluedroid/app	\
		bluedroid/gki	\
		bluedroid/bta/gatt	\
		bluedroid/bta/dm	\
		bluedroid/bta/sys	\
		bluedroid/bta	\
		bluedroid/osi	\
		bluedroid/test	\
		bluedroid/btif	\
		bluedroid	\
		./

include $(IDF_PATH)/make/component_common.mk
