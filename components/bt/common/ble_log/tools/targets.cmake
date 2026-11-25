idf_build_get_property(ble_log_python PYTHON)
idf_build_get_property(ble_log_project_dir PROJECT_DIR)

foreach(tool IN ITEMS ble-log-config-assistant ble-log-console)
    add_custom_target(${tool}
        COMMAND "${ble_log_python}" "${CMAKE_CURRENT_LIST_DIR}/download_tools.py" "${tool}"
        WORKING_DIRECTORY "${ble_log_project_dir}"
        USES_TERMINAL
        COMMENT "Start ${tool}"
        VERBATIM
    )
endforeach()
