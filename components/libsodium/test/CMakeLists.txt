if(TESTS_ALL EQUAL 1)
    message("not linking libsodium tests, use '-T libsodium' to test it")
else()
    get_filename_component(LS_TESTDIR "${CMAKE_CURRENT_LIST_DIR}/../libsodium/test/default" ABSOLUTE)

    set(TEST_CASES "chacha20;aead_chacha20poly1305;box;box2;ed25519_convert;sign;hash")

    foreach(test_case ${TEST_CASES})
        file(GLOB test_case_file "${LS_TESTDIR}/${test_case}.c")
        list(APPEND TEST_CASES_FILES ${test_case_file})
    endforeach()

    idf_component_register(SRCS "${TEST_CASES_FILES}" "test_sodium.c"
                        PRIV_INCLUDE_DIRS "." "${LS_TESTDIR}/../quirks"
                        PRIV_REQUIRES cmock libsodium)

    # The libsodium test suite is designed to be run each test case as an executable on a desktop computer and uses
    # filesytem to write & then compare contents of each file.
    #
    # For now, use their "BROWSER_TEST" mode with these hacks so that
    # multiple test cases can be combined into one ELF file.
    #
    # Run each test case from test_sodium.c as CASENAME_xmain().
    foreach(test_case_file ${TEST_CASES_FILES})
        get_filename_component(test_case ${test_case_file} NAME_WE)
        set_source_files_properties(${test_case_file}
                                    PROPERTIES COMPILE_FLAGS
                                    # This would generate 'warning "main" redefined' warnings at runtime, which are
                                    # silenced here. Only other solution involves patching libsodium's cmptest.h.
                                    "-Dxmain=${test_case}_xmain -Dmain=${test_case}_main -Wp,-w")
    endforeach()

    # this seems odd, but it prevents the libsodium test harness from
    # trying to write to a file!
    add_definitions(-DBROWSER_TESTS)
endif()
