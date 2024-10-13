# idf_create_lcov_report
#
# Create coverage report.
function(idf_create_coverage_report report_dir)
    set(gcov_tool ${_CMAKE_TOOLCHAIN_PREFIX}gcov)
    idf_build_get_property(project_name PROJECT_NAME)

    file(TO_NATIVE_PATH "${report_dir}" _report_dir)
    file(TO_NATIVE_PATH "${project_dir}" _project_dir)
    file(TO_NATIVE_PATH "${report_dir}/html/index.html" _index_path)

    add_custom_target(pre-cov-report
        COMMENT "Generating coverage report in: ${_report_dir}"
        COMMAND ${CMAKE_COMMAND} -E echo "Using gcov: ${gcov_tool}"
        COMMAND ${CMAKE_COMMAND} -E make_directory ${_report_dir}/html
        )

    add_custom_target(lcov-report
        COMMENT "WARNING: lcov-report is deprecated. Please use gcovr-report instead."
        COMMAND lcov --gcov-tool ${gcov_tool} -c -d ${CMAKE_CURRENT_BINARY_DIR} -o ${_report_dir}/${project_name}.info
        COMMAND genhtml -o ${_report_dir}/html ${_report_dir}/${project_name}.info
        DEPENDS pre-cov-report
        )

    add_custom_target(gcovr-report
        COMMAND gcovr -r ${_project_dir} --gcov-executable ${gcov_tool} -s --html-details ${_index_path}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS pre-cov-report
        )
endfunction()

# idf_clean_coverage_report
#
# Clean coverage report.
function(idf_clean_coverage_report report_dir)
    file(TO_CMAKE_PATH "${report_dir}" _report_dir)

    add_custom_target(cov-data-clean
        COMMENT "Clean coverage report in: ${_report_dir}"
        COMMAND ${CMAKE_COMMAND} -E remove_directory ${_report_dir})
endfunction()
