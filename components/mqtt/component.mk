COMPONENT_SUBMODULES += esp-mqtt
COMPONENT_ADD_INCLUDEDIRS := esp-mqtt/include
COMPONENT_SRCDIRS :=  esp-mqtt esp-mqtt/lib
COMPONENT_PRIV_INCLUDEDIRS := esp-mqtt/lib/include

ifeq ($(GCC_NOT_5_2_0), 1)
esp-mqtt/lib/transport_ws.o: CFLAGS += -Wno-format-overflow
endif
