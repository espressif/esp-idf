idf_component_register(SRCS "ulp_riscv_example_main.c"
                       REQUIRES ulp)

ulp_add_project("ulp_main" "${CMAKE_CURRENT_LIST_DIR}/ulp/" TYPE riscv)
