# This component provides the logging utility functions.

# Explicitly pull in the IDF log component before using it.
idf_component_include(log)

add_library(${COMPONENT_TARGET} STATIC logging_util.c)
target_include_directories(${COMPONENT_TARGET} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(${COMPONENT_TARGET} PRIVATE idf::log)
