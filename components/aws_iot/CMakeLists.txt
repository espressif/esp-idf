if(CONFIG_AWS_IOT_SDK)
    set(COMPONENT_ADD_INCLUDEDIRS "include aws-iot-device-sdk-embedded-C/include")
    set(COMPONENT_SRCDIRS "aws-iot-device-sdk-embedded-C/src port")
else()
    message(STATUS "Building empty aws_iot component due to configuration")
endif()

set(COMPONENT_REQUIRES "mbedtls")
set(COMPONENT_PRIV_REQUIRES "jsmn")

register_component()
