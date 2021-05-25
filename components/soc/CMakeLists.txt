set(SOC_NAME ${IDF_TARGET})

if(EXISTS "${COMPONENT_PATH}/${SOC_NAME}")
    include(${COMPONENT_PATH}/${SOC_NAME}/sources.cmake)

    spaces2list(SOC_SRCS)
    add_prefix(COMPONENT_SRCS "${SOC_NAME}/" ${SOC_SRCS})
    set(COMPONENT_ADD_INCLUDEDIRS ${SOC_NAME}/include ${SOC_NAME}/)
endif()

list(APPEND COMPONENT_ADD_INCLUDEDIRS include)
list(APPEND COMPONENT_SRCS "src/memory_layout_utils.c")

if (NOT(BOOTLOADER_BUILD))
    list(APPEND COMPONENT_SRCS "src/regi2c_ctrl.c")
endif()

set(COMPONENT_ADD_LDFRAGMENTS linker.lf)

set(COMPONENT_REQUIRES)

register_component()
