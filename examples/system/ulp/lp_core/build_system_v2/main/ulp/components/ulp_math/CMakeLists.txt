add_library(${COMPONENT_TARGET} STATIC "ulp_math.c")
target_include_directories(${COMPONENT_TARGET} PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/include)

# Static linker script — defines a simple symbol
idf_component_set_property(${COMPONENT_NAME} LINKER_SCRIPTS ulp_math_sections.ld APPEND)

# Linker fragment file for ldgen — describes custom section placement
idf_component_set_property(${COMPONENT_NAME} LDFRAGMENTS ulp_math.lf)
