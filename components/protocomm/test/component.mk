COMPONENT_PRIV_INCLUDEDIRS := ../proto-c/
COMPONENT_ADD_LDFLAGS = -Wl,--whole-archive -l$(COMPONENT_NAME) -Wl,--no-whole-archive
