#
#Component Makefile
#

COMPONENT_SRCDIRS := .

COMPONENT_ADD_LDFLAGS = -Wl,--whole-archive -l$(COMPONENT_NAME) -Wl,--no-whole-archive

COMPONENT_EMBED_TXTFILES := dl_espressif_com_root_cert.pem
