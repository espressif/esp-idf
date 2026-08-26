set(srcs
    "counter.S")

idf_component_register(SRCS ${srcs}
                       REQUIRES ulp ulp_wake
                       LDFRAGMENTS "ulp_fsm_sections.lf")

target_compile_definitions(${COMPONENT_LIB} PRIVATE ULP_BUILD_SYSTEM_FSM_EXAMPLE=1)

# Supplementary linker script template processed by ldgen. The mapping marker
# is resolved using ulp_fsm_sections.lf from this component.
target_linker_script(${COMPONENT_NAME} INTERFACE ulp_sections.ld.in
                     PROCESS "${CMAKE_BINARY_DIR}/ulp_sections.ld")
