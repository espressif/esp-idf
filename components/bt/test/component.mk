ifdef CONFIG_BT_ENABLED
COMPONENT_ADD_LDFLAGS = -Wl,--whole-archive -l$(COMPONENT_NAME) -Wl,--no-whole-archive
else
COMPONENT_CONFIG_ONLY := 1
endif
