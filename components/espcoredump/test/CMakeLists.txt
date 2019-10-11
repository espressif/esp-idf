if(TESTS_ALL EQUAL 1)
    message("not linking coredump test from CI.")
else()
    idf_component_register(SRC_DIRS "."
                        INCLUDE_DIRS "."
                        REQUIRES unity nvs_flash)
endif()