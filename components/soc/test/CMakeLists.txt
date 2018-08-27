set(SOC_NAME esp32)
set(COMPONENT_SRCDIRS "../${SOC_NAME}/test")
set(COMPONENT_ADD_INCLUDEDIRS "../${SOC_NAME}/test")

set(COMPONENT_REQUIRES unity)

register_component()