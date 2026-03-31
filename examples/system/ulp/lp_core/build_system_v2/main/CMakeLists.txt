idf_component_register(SRCS "lp_core_build_system_example_main.c"
                       REQUIRES ulp
                       WHOLE_ARCHIVE)

ulp_add_project("ulp_build_system_example" "${CMAKE_CURRENT_LIST_DIR}/ulp/")
