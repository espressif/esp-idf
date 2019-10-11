COMPONENT_SRCDIRS := src proto-c
COMPONENT_ADD_INCLUDEDIRS := include
COMPONENT_PRIV_INCLUDEDIRS := src proto-c ../protocomm/proto-c/

ifndef CONFIG_BT_BLUEDROID_ENABLED
	COMPONENT_OBJEXCLUDE += src/esp_local_ctrl_transport_ble.o
endif

ifndef CONFIG_ESP_HTTPS_SERVER_ENABLE
	COMPONENT_OBJEXCLUDE += src/esp_local_ctrl_transport_httpd.o
endif
