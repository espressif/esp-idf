# post_build_validation.cmake
#
# Functions for post-build validation and checks

#
# Setup post-build validation checks
#
function(__idf_build_setup_post_build_validation)
    # Create the following post-build targets after __idf_build_binary() is called to ensure that the
    # app target is available.

    # If anti-rollback option is set then factory partition should not be in Partition Table.
    # In this case, should be used the partition table with two ota app without the factory.
    partition_table_get_partition_info(factory_offset
                    "--partition-type app --partition-subtype factory" "offset")
    partition_table_get_partition_info(test_offset "--partition-type app --partition-subtype test" "offset")
    if(CONFIG_BOOTLOADER_APP_ANTI_ROLLBACK AND (factory_offset OR test_offset))
        fail_at_build_time(check_table_contents "\
        ERROR: Anti-rollback option is enabled. Partition table should \
        consist of two ota app without factory or test partitions.")
        add_dependencies(app check_table_contents)
    endif()

    if(CONFIG_APP_BUILD_TYPE_APP_2NDBOOT)
        # Generate app_check_size_command target to check the app size against the partition table parameters
        idf_build_get_property(build_dir BUILD_DIR)
        idf_build_get_property(project_bin PROJECT_BIN)
        partition_table_add_check_size_target(app_check_size
            DEPENDS gen_project_binary
            BINARY_PATH "${build_dir}/${project_bin}"
            PARTITION_TYPE app)
        add_dependencies(app app_check_size)
    endif()
endfunction()
