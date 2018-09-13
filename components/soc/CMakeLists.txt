set(SOC_NAME esp32)

set(COMPONENT_ADD_INCLUDEDIRS ${SOC_NAME}/include include)

include(${IDF_PATH}/components/soc/${SOC_NAME}/sources.cmake)

spaces2list(SOC_SRCS)
add_prefix(COMPONENT_SRCS "${SOC_NAME}/" ${SOC_SRCS})

set(COMPONENT_ADD_INCLUDEDIRS ${SOC_NAME}/include include)

set(COMPONENT_REQUIRES)
register_component()