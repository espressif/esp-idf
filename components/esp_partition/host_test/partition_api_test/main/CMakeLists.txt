idf_component_register(SRCS "partition_api_test.c"
                       PRIV_REQUIRES esp_partition unity spi_flash)

# set BUILD_DIR because test uses a file created in the build directory
target_compile_definitions(${COMPONENT_LIB} PRIVATE "BUILD_DIR=\"${build_dir}\"")
