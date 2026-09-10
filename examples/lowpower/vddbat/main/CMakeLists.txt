set(srcs "vddbat_example_main.c")
set(includes ".")

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${includes})
