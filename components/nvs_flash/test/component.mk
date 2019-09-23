#
#Component Makefile
#

COMPONENT_ADD_LDFLAGS = -Wl,--whole-archive -l$(COMPONENT_NAME) -Wl,--no-whole-archive
COMPONENT_EMBED_TXTFILES += encryption_keys.bin partition_encrypted.bin sample.bin
