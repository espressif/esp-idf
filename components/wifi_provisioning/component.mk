COMPONENT_SRCDIRS := src proto-c
COMPONENT_ADD_INCLUDEDIRS := include
COMPONENT_PRIV_INCLUDEDIRS := src proto-c ../protocomm/proto-c/

ifneq ($(filter y, $(CONFIG_BT_ENABLED) $(CONFIG_BLUEDROID_ENABLED)),y y)
	COMPONENT_OBJEXCLUDE := src/scheme_ble.o
endif
