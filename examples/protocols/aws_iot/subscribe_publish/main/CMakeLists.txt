set(COMPONENT_SRCS "subscribe_publish_sample.c")
set(COMPONENT_ADD_INCLUDEDIRS ".")


register_component()

if(CONFIG_EXAMPLE_EMBEDDED_CERTS)
target_add_binary_data(${COMPONENT_TARGET} "certs/aws-root-ca.pem" TEXT)

if(NOT IDF_CI_BUILD)
    add_custom_command(OUTPUT certs/certificate.pem.crt certs/private.pem.key
                        COMMAND echo "Dummy certificate data for continuous integration" >
                                certs/certificate.pem.crt
                        COMMAND echo "Dummy certificate data for continuous integration" >
                                certs/private.pem.key
                        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                        VERBATIM)
    add_custom_target(example_certificates DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/certs/certificate.pem.crt ${CMAKE_CURRENT_BINARY_DIR}/certs/private.pem.key)
    
    add_dependencies(${COMPONENT_TARGET} example_certificates)
    
    target_add_binary_data(${COMPONENT_TARGET} "${CMAKE_CURRENT_BINARY_DIR}/certs/certificate.pem.crt" TEXT)
    target_add_binary_data(${COMPONENT_TARGET} "${CMAKE_CURRENT_BINARY_DIR}/certs/private.pem.key" TEXT)
else()
    target_add_binary_data(${COMPONENT_TARGET} "certs/certificate.pem.crt" TEXT)
    target_add_binary_data(${COMPONENT_TARGET} "certs/private.pem.key" TEXT) 
endif()
endif()