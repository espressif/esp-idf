COMPONENT_ADD_INCLUDEDIRS := include/common include/security include/transports
COMPONENT_PRIV_INCLUDEDIRS := proto-c src/common src/simple_ble
COMPONENT_SRCDIRS := src/common src/security proto-c src/simple_ble src/transports

ifndef CONFIG_BT_BLUEDROID_ENABLED
	COMPONENT_OBJEXCLUDE += src/simple_ble/simple_ble.o src/transports/protocomm_ble.o
endif

ifndef CONFIG_BT_NIMBLE_ENABLED
       COMPONENT_OBJEXCLUDE += src/transports/protocomm_nimble.o
endif
