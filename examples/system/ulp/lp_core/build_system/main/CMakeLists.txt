# Set usual component variables
set(app_sources "lp_core_build_system_example_main.c")

idf_component_register(SRCS ${app_sources}
                       REQUIRES ulp
                       WHOLE_ARCHIVE)

ulp_add_project("ulp_build_system_example" "${CMAKE_SOURCE_DIR}/main/ulp/")
