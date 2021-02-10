#
# Main Makefile. This is basically the same as a component makefile.
#

ifdef CONFIG_EXAMPLE_OPENSSL_CLIENT_URI_FROM_STDIN
COMPONENT_EMBED_TXTFILES := ${PROJECT_PATH}/server_certs/ca.crt
else
COMPONENT_EMBED_TXTFILES := ${PROJECT_PATH}/main/baidu_ca.crt
endif
COMPONENT_EMBED_TXTFILES += ${PROJECT_PATH}/server_certs/ca.key
