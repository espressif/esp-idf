set(COMPONENT_SRCS "native_ota_example.c")
set(COMPONENT_ADD_INCLUDEDIRS ".")

# Embed the server root certificate into the final binary
idf_build_get_property(project_dir PROJECT_DIR)
set(COMPONENT_EMBED_TXTFILES ${project_dir}/server_certs/ca_cert.pem)

register_component()
