idf_component_register(SRCS "cxx_exception_stubs.cpp"
                            "cxx_guards.cpp"
                        # Make sure that pthread is in component list
                        PRIV_REQUIRES pthread)

target_link_libraries(${COMPONENT_LIB} PUBLIC stdc++ gcc)
target_link_libraries(${COMPONENT_LIB} INTERFACE "-u __cxa_guard_dummy")

# Force pthread to also appear later than stdc++ in link line
add_library(stdcpp_pthread INTERFACE)
idf_component_get_property(pthread pthread COMPONENT_LIB)
target_link_libraries(stdcpp_pthread INTERFACE stdc++ $<TARGET_FILE:${pthread}>)
target_link_libraries(${COMPONENT_LIB} PUBLIC stdcpp_pthread)

if(NOT CONFIG_COMPILER_CXX_EXCEPTIONS)
    target_link_libraries(${COMPONENT_LIB} INTERFACE "-u __cxx_fatal_exception")
endif()

if(NOT CONFIG_COMPILER_CXX_RTTI)
    target_link_libraries(${COMPONENT_LIB} PUBLIC -fno-rtti)
endif()
