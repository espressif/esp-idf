idf_component_register(SRCS "test_nvs.cpp"
                            "test_partition_manager.cpp"
                            "test_nvs_cxx_api.cpp"
                            "test_nvs_handle.cpp"
                            "test_nvs_initialization.cpp"
                            "test_nvs_storage.cpp"
                       INCLUDE_DIRS
                            "../../../src"
                            "../../../private_include"
                            "../../../../mbedtls/mbedtls/include"
                       WHOLE_ARCHIVE
                       REQUIRES nvs_flash)

if(CMAKE_C_COMPILER_ID MATCHES "Clang")
    target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu++20)
endif()

# Currently 'main' for IDF_TARGET=linux is defined in freertos component.
# Since we are using a freertos mock here, need to let Catch2 provide 'main'.
target_link_libraries(${COMPONENT_LIB} PRIVATE Catch2WithMain)
