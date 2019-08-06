COMPONENT_ADD_LDFLAGS = -Wl,--whole-archive -l$(COMPONENT_NAME) -Wl,--no-whole-archive
COMPONENT_EMBED_TXTFILES := fatfs.img
COMPONENT_SRCDIRS += esp32
