idf_component_register(SRCS "ulp_combined_build_system_example_main.c"
                       REQUIRES ulp)

ulp_add_project("ulp_fsm_main" "${CMAKE_CURRENT_LIST_DIR}/ulp_fsm/" TYPE fsm)
ulp_add_project("ulp_riscv_main" "${CMAKE_CURRENT_LIST_DIR}/ulp_riscv/" TYPE riscv)
