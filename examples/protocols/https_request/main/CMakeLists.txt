# Embed the server root certificate into the final binary
#
# (If this was a component, we would set COMPONENT_EMBED_TXTFILES here.)
idf_component_register(SRCS "https_request_example_main.c" "time_sync.c"
                    INCLUDE_DIRS "include"
                    EMBED_TXTFILES server_root_cert.pem local_server_cert.pem)
