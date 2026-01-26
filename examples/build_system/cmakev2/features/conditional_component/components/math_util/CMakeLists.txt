# This component provides the math utility functions.
add_library(${COMPONENT_TARGET} STATIC math_util.c)
target_include_directories(${COMPONENT_TARGET} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
