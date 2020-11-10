#
#Component Makefile
#

COMPONENT_ADD_LDFLAGS = -Wl,--whole-archive -l$(COMPONENT_NAME) -Wl,--no-whole-archive
COMPONENT_EMBED_FILES := server_cert_chain.pem prvtkey.pem server_cert_bundle
