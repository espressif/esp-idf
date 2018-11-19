set(SOC_NAME ${IDF_TARGET})
if(EXISTS "../${SOC_NAME}/test")
    set(COMPONENT_SRCDIRS "../${SOC_NAME}/test")
    set(COMPONENT_ADD_INCLUDEDIRS "../${SOC_NAME}/test")

    set(COMPONENT_REQUIRES unity test_utils)

    register_component()
endif()
