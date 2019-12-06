COMPONENT_SRCDIRS := src proto-c
COMPONENT_ADD_INCLUDEDIRS := include
COMPONENT_PRIV_INCLUDEDIRS := src proto-c ../protocomm/proto-c/

# To avoid warning for strncpy in "handlers.c" and "scheme_softap.c"
CPPFLAGS += -Wno-stringop-truncation

ifndef CONFIG_BT_BLUEDROID_ENABLED
    ifndef CONFIG_BT_NIMBLE_ENABLED
        COMPONENT_OBJEXCLUDE := src/scheme_ble.o
    endif
endif

