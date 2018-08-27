ULP_APP_NAME = ulp_test_app

ULP_S_SOURCES = $(addprefix $(COMPONENT_PATH)/ulp/, \
	test_jumps.S \
	)

ULP_EXP_DEP_OBJECTS := test_ulp_as.o
 
include $(IDF_PATH)/components/ulp/component_ulp_common.mk

COMPONENT_ADD_LDFLAGS += -Wl,--whole-archive -l$(COMPONENT_NAME) -Wl,--no-whole-archive
