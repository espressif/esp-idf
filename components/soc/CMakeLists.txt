set(SOC_NAME esp32)

set(COMPONENT_SRCDIRS ${SOC_NAME} src)
set(COMPONENT_ADD_INCLUDEDIRS ${SOC_NAME}/include include)

set(COMPONENT_REQUIRES)


register_component()
