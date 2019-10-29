COMPONENT_ADD_LDFLAGS = -Wl,--whole-archive -l$(COMPONENT_NAME) -Wl,--no-whole-archive
COMPONENT_EMBED_FILES := test_partition_v1.bin
COMPONENT_SRCDIRS += esp32
