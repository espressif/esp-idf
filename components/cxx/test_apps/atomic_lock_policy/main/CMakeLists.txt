idf_component_register(SRCS "test_cxx_atomic_lock_policy.cpp"
                            "cxx_atomic_a.cpp"
                            "cxx_atomic_b.cpp")

# Default build: both TUs inherit CONFIG_COMPILER_CXX_ATOMIC_LOCK_POLICY.
# With -DTEST_INVALID_LOCK_POLICIES=1, undefine the macro for one TU so the
# linked image mixes __gnu_cxx::_Lock_policy instantiations; the cxx post-build
# hook (check_cxx_atomic_policy.cmake) must then fail the build.
if(TEST_INVALID_LOCK_POLICIES)
    set_source_files_properties("cxx_atomic_b.cpp" PROPERTIES
        COMPILE_OPTIONS "-U_GLIBCXX_HAVE_ATOMIC_LOCK_POLICY")
endif()
