set(srcs "test_app_main.c"
         "test_rgb_panel.c")

idf_component_register(SRCS ${srcs})

target_link_libraries(${COMPONENT_LIB} INTERFACE "-u test_app_include_rgb_lcd")
