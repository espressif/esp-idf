idf_component_register(SRCS "test_zcmp_workaround_checking.c")

if(TEST_INVALID_WORKAROUND)
    target_compile_definitions(${COMPONENT_LIB} PRIVATE TEST_INVALID_WORKAROUND)
endif()
