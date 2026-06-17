idf_component_register(SRCS "ulp_multi_binary_example_main.c"
                       REQUIRES ulp)

ulp_add_project("ulp_multi_binary" "${CMAKE_CURRENT_LIST_DIR}/ulp/"
                BINARIES add multiply)
