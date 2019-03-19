set(SOC_NAME ${IDF_TARGET})
set(SOC_TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../${SOC_NAME}/test)
if(EXISTS "${SOC_TEST_DIR}")
    set(COMPONENT_SRCDIRS "${SOC_TEST_DIR}")
    set(COMPONENT_ADD_INCLUDEDIRS "${SOC_TEST_DIR}")
endif()

set(COMPONENT_REQUIRES unity test_utils)

register_component()
