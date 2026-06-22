idf_component_register(SRCS "ft_demo.c" "ft_demo_hot.c" "ft_demo_quiet.c"
                       INCLUDE_DIRS "include")

# Opt the whole component into compiler-instrumented function tracing by adding
# -finstrument-functions to its build. The guard keeps the flag out when the
# feature is off, otherwise the __cyg_profile_* hooks would be undefined at link.
# The exclude-list flags skip specific files / functions:
#  - ft_demo_hot.c and ft_demo_quiet.c are excluded by file (comma-separated)
#  - ft_demo_secret is excluded by function name
if(CONFIG_ESP_TRACE_FUNCTION_TRACE)
    target_compile_options(${COMPONENT_LIB} PRIVATE
        -finstrument-functions
        -finstrument-functions-exclude-file-list=ft_demo_hot,ft_demo_quiet
        -finstrument-functions-exclude-function-list=ft_demo_secret)
endif()
