if(CONFIG_AWS_IOT_SDK)

set(COMPONENT_ADD_INCLUDEDIRS "include aws-iot-device-sdk-embedded-C/include")
set(COMPONENT_SRCDIRS "aws-iot-device-sdk-embedded-C/src port")
set(COMPONENT_SUBMODULES aws-iot-device-sdk-embedded-C)

register_component()

endif(CONFIG_AWS_IOT_SDK)
