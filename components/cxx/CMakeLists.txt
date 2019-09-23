idf_component_register(SRCS "cxx_exception_stubs.cpp"
                            "cxx_guards.cpp")

target_link_libraries(${COMPONENT_LIB} PUBLIC stdc++ gcc)
target_link_libraries(${COMPONENT_LIB} INTERFACE "-u __cxa_guard_dummy")

if(NOT CONFIG_COMPILER_CXX_EXCEPTIONS)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u __cxx_fatal_exception")
endif()
