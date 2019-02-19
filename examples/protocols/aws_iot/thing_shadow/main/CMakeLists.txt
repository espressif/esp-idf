set(COMPONENT_SRCS "thing_shadow_sample.c")
set(COMPONENT_ADD_INCLUDEDIRS ".")


register_component()

if(CONFIG_EXAMPLE_EMBEDDED_CERTS)
target_add_binary_data(${COMPONENT_NAME} "certs/aws-root-ca.pem" TEXT)
target_add_binary_data(${COMPONENT_NAME} "certs/certificate.pem.crt" TEXT)
target_add_binary_data(${COMPONENT_NAME} "certs/private.pem.key" TEXT)
endif()
