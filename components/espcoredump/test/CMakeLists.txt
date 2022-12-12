if(TESTS_ALL EQUAL 1)
    message("not linking coredump test from CI.")
else()
    idf_component_register(SRC_DIRS "."
                        PRIV_INCLUDE_DIRS "."
                        PRIV_REQUIRES cmock nvs_flash test_utils)
    target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
endif()
