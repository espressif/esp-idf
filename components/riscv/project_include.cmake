# avoid esp-clang "unused-command-line-argument" warning
if(NOT (CMAKE_C_COMPILER_ID MATCHES "Clang") )
    set(compile_options "-nostartfiles")
    idf_build_set_property(COMPILE_OPTIONS "${compile_options}" APPEND)
endif()
