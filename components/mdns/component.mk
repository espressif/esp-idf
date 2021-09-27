ifdef CONFIG_MDNS_NETWORKING_SOCKET
COMPONENT_OBJEXCLUDE := mdns_networking_lwip.o
else
COMPONENT_OBJEXCLUDE := mdns_networking_socket.o
endif
COMPONENT_ADD_INCLUDEDIRS := include
COMPONENT_PRIV_INCLUDEDIRS := private_include
