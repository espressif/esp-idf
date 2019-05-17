set(COMPONENT_SRCS "cxx_exception_stubs.cpp"
                   "cxx_guards.cpp")
register_component()

target_link_libraries(${COMPONENT_LIB} stdc++)
target_link_libraries(${COMPONENT_LIB} "-u __cxa_guard_dummy")

if(NOT CONFIG_CXX_EXCEPTIONS)
    target_link_libraries(${COMPONENT_LIB} "-u __cxx_fatal_exception")
endif()
