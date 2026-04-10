# This component is not supported by ESP targets
if(${target} not STREQUAL "linux")
    return()
endif()

idf_component_register(
    SRCS src/safe_unity.c
    INCLUDE_DIRS include
    REQUIRES unity
)

target_compile_options(${COMPONENT_LIB} PUBLIC --coverage)
target_link_libraries(${COMPONENT_LIB} PUBLIC --coverage)
