set(SOC_NAME esp32)

include(${IDF_PATH}/components/soc/${SOC_NAME}/sources.cmake)

spaces2list(SOC_SRCS)
add_prefix(COMPONENT_SRCS "${SOC_NAME}/" ${SOC_SRCS})
list(APPEND COMPONENT_SRCS "src/memory_layout_utils.c")

set(COMPONENT_ADD_INCLUDEDIRS ${SOC_NAME}/include include)

set(COMPONENT_REQUIRES)
register_component()