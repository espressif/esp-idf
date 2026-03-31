add_library(${COMPONENT_TARGET} STATIC "ulp_sum.c")
target_include_directories(${COMPONENT_TARGET} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include)

# ulp_clamp is resolved by the component manager via idf_component.yml
idf_component_include(ulp_clamp)
target_link_libraries(${COMPONENT_TARGET} PRIVATE idf::ulp_clamp)
