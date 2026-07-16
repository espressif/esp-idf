idf_component_register(SRCS "main.c"
                       REQUIRES ulp
                       LDFRAGMENTS "ulp_riscv_sections.lf")

# Supplementary linker script template processed by ldgen. The mapping marker
# is resolved using ulp_riscv_sections.lf from this component.
target_linker_script(${COMPONENT_NAME} INTERFACE ulp_sections.ld.in
                     PROCESS "${CMAKE_CURRENT_BINARY_DIR}/ulp_sections.ld")
