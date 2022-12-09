COMPONENT_SRCDIRS := src proto-c
COMPONENT_ADD_INCLUDEDIRS := include
COMPONENT_PRIV_INCLUDEDIRS := src proto-c ../protocomm/proto-c/

ifndef CONFIG_BT_BLUEDROID_ENABLED
    ifndef CONFIG_BT_NIMBLE_ENABLED
        COMPONENT_OBJEXCLUDE := src/scheme_ble.o
    endif
endif

