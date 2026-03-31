add_library(${COMPONENT_TARGET} STATIC "main.c")

idf_component_include(ulp_sum)
idf_component_include(ulp_math)

target_link_libraries(${COMPONENT_TARGET} PRIVATE idf::ulp_sum idf::ulp_math)

# Linker script template processed by ldgen. The mapping[ulp_math_fast]
# marker is resolved using the .lf fragment from the ulp_math component.
target_linker_script(${COMPONENT_NAME} INTERFACE ulp_sections.ld.in
                     PROCESS ulp_sections.ld)
