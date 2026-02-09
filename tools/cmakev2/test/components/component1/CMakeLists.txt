add_library(${COMPONENT_TARGET} component1.c)
target_include_directories(${COMPONENT_TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
