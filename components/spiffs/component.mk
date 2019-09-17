COMPONENT_ADD_INCLUDEDIRS := include
COMPONENT_PRIV_INCLUDEDIRS := . spiffs/src
COMPONENT_SRCDIRS := . spiffs/src

# To avoid warning for strncpy in "spiffs_nucleus.c"
CPPFLAGS += -Wno-stringop-truncation

COMPONENT_SUBMODULES := spiffs
