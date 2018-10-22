set(COMPONENT_SRCDIRS ".")
set(COMPONENT_ADD_INCLUDEDIRS ".")

set(COMPONENT_REQUIRES unity ulp soc)

register_component()

set(ULP_APP_NAME ulp_test_app)
set(ULP_S_SOURCES "ulp/test_jumps.S")
set(ULP_EXP_DEP_SRCS "test_ulp_as.c")
include(${IDF_PATH}/components/ulp/component_ulp_common.cmake)
