#
# Main Makefile. This is basically the same as a component makefile.
#

# embed files from the "certs" directory as binary data symbols
# in the app
COMPONENT_EMBED_TXTFILES := server_root_cert.pem

include $(IDF_PATH)/make/component_common.mk
