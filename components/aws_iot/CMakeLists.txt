if(NOT CONFIG_AWS_IOT_SDK)
    return()
endif()

set(COMPONENT_ADD_INCLUDEDIRS "include aws-iot-device-sdk-embedded-C/include")
set(COMPONENT_SRCDIRS "aws-iot-device-sdk-embedded-C/src port")

register_component()
