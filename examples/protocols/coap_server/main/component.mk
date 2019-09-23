#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

# embed files from the "certs" directory as binary data symbols
# in the app
COMPONENT_EMBED_TXTFILES := certs/coap_ca.pem certs/coap_server.crt certs/coap_server.key
