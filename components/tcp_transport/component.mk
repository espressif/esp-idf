COMPONENT_ADD_INCLUDEDIRS := include
COMPONENT_PRIV_INCLUDEDIRS := private_include

ifndef CONFIG_WS_TRANSPORT
	COMPONENT_OBJEXCLUDE := transport_ws.o
endif
