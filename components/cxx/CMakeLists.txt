set(COMPONENT_SRCS "cxx_exception_stubs.cpp"
                   "cxx_guards.cpp")
set(COMPONENT_REQUIRES)
register_component()

target_link_libraries(cxx stdc++)

target_link_libraries(cxx "-u __cxa_guard_dummy")

if(NOT CONFIG_CXX_EXCEPTIONS)
    target_link_libraries(cxx "-u __cxx_fatal_exception")
endif()
