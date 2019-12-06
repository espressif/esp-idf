# idf_create_lcov_report
#
# Create coverage report.
function(idf_create_coverage_report report_dir)
    set(gcov_tool ${CONFIG_SDK_TOOLPREFIX}gcov)
    idf_build_get_property(project_name PROJECT_NAME)

    add_custom_target(lcov-report
        COMMENT "Generating coverage report in: ${report_dir}"
        COMMAND ${CMAKE_COMMAND} -E echo "Using gcov: ${gcov_tool}"
        COMMAND ${CMAKE_COMMAND} -E make_directory ${report_dir}/html
        COMMAND lcov --gcov-tool ${gcov_tool} -c -d ${CMAKE_CURRENT_BINARY_DIR} -o ${report_dir}/${project_name}.info
        COMMAND genhtml -o ${report_dir}/html ${report_dir}/${project_name}.info)
endfunction()

# idf_clean_coverage_report
#
# Clean coverage report.
function(idf_clean_coverage_report report_dir)
    add_custom_target(cov-data-clean
        COMMENT "Clean coverage report in: ${report_dir}"
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${report_dir})
endfunction()