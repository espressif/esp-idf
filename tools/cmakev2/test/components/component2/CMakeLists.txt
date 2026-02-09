idf_component_include(component1 INTERFACE component1)

add_library(${COMPONENT_TARGET} component2.c)
target_include_directories(${COMPONENT_TARGET} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(${COMPONENT_TARGET} PRIVATE ${component1})
