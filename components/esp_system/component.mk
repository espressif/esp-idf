SOC_NAME := $(IDF_TARGET)

COMPONENT_SRCDIRS := .
COMPONENT_ADD_INCLUDEDIRS := include
COMPONENT_PRIV_INCLUDEDIRS := private_include port/include
COMPONENT_ADD_LDFRAGMENTS += linker.lf

include $(COMPONENT_PATH)/port/soc/$(SOC_NAME)/component.mk

# disable stack protection in files which are involved in initialization of that feature
startup.o: CFLAGS := $(filter-out -fstack-protector%, $(CFLAGS))
