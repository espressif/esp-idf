idf_component_register(SRCS "main.c"
                       REQUIRES ulp ulp_sum ulp_math)

# Linker script template processed by ldgen. The mapping[ulp_math_fast]
# marker is resolved using the .lf fragment from the ulp_math component.
target_linker_script(${COMPONENT_NAME} INTERFACE ulp_sections.ld.in
                     PROCESS "${CMAKE_CURRENT_BINARY_DIR}/ulp_sections.ld")
