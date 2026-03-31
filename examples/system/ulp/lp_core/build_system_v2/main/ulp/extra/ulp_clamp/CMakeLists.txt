add_library(${COMPONENT_TARGET} STATIC "ulp_clamp.c")
target_include_directories(${COMPONENT_TARGET} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include)
