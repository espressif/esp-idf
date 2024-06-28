# Set usual component variables
set(app_sources "lp_debug_main.c")

idf_component_register(SRCS ${app_sources}
                       REQUIRES ulp
                       WHOLE_ARCHIVE)

ulp_add_project("ulp_debug_example" "${CMAKE_SOURCE_DIR}/main/ulp/")
