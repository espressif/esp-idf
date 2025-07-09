
set(LOG_COMPRESSED_MODULE "")
set(LOG_COMPRESSED_MODULE_CODE_PATH "")
set(LOG_COMPRESSED_SRCS_DIR "${CMAKE_BINARY_DIR}/ble_log/.compressed_srcs")

# default config value for ble mesh module
set(BLE_MESH_CODE_PATH "")
set(BLE_MESH_LOG_INDEX_HEADER "\"\"")
set(BLE_MESH_TAGS "")
set(BLE_MESH_TAGS_PRESERVE "")

# default config value for host module
set(HOST_CODE_PATH "")
set(HOST_LOG_INDEX_HEADER "\"\"")
set(BLE_HOST_TAGS "")
set(BLE_HOST_TAGS_PRESERVE "")

if(CONFIG_BLE_MESH_COMPRESSED_LOG_ENABLE)
    list(APPEND LOG_COMPRESSED_MODULE "BLE_MESH")
    if(NOT EXISTS "${CMAKE_BINARY_DIR}/ble_log/include/mesh_log_index.h")
        file(WRITE "${CMAKE_BINARY_DIR}/ble_log/include/mesh_log_index.h" "")
    endif()
    list(APPEND LOG_COMPRESSED_MODULE_CODE_PATH "esp_ble_mesh")

    # update config file
    set(BLE_MESH_CODE_PATH "esp_ble_mesh")
    set(BLE_MESH_LOG_INDEX_HEADER "mesh_log_index.h")
    # update BLE_MESH_TAGS and BLE_MESH_TAGS_PRESERVE
    include(${CMAKE_CURRENT_LIST_DIR}/cmake/ble_mesh_log_tags.cmake)

endif()
if(CONFIG_BLE_HOST_COMPRESSED_LOG_ENABLE AND CONFIG_BT_BLUEDROID_ENABLED)
    list(APPEND LOG_COMPRESSED_MODULE "BLE_HOST")
    list(APPEND LOG_COMPRESSED_MODULE_CODE_PATH "host/bluedroid/stack")
    if(NOT EXISTS "${CMAKE_BINARY_DIR}/ble_log/include/host_log_index.h")
        file(WRITE "${CMAKE_BINARY_DIR}/ble_log/include/host_log_index.h" "")
    endif()

    set(HOST_CODE_PATH "host/bluedroid/stack")
    set(HOST_LOG_INDEX_HEADER "host_log_index.h")
    include(${CMAKE_CURRENT_LIST_DIR}/cmake/ble_host_bluedroid_tags.cmake)
endif()
if(LOG_COMPRESSED_MODULE)
    list(APPEND srcs "common/ble_log/extension/log_compression/ble_log_compression.c")
    list(APPEND include_dirs "${CMAKE_BINARY_DIR}/ble_log/include")
    if(NOT CMAKE_VERSION VERSION_LESS 3.15.0)
        set(Python3_FIND_STRATEGY LOCATION)
        find_package(Python3 COMPONENTS Interpreter)
        if(Python3_Interpreter_FOUND)
            set(BLE_PYTHON_EXECUTABLE ${Python3_EXECUTABLE})
        endif()
    else()
        find_package(PythonInterp 3)
        if(PYTHONINTERP_FOUND)
            set(BLE_PYTHON_EXECUTABLE ${PYTHON_EXECUTABLE})
        endif()
    endif()

    if(BLE_PYTHON_EXECUTABLE)
        set(PYTHON_SCRIPT ${BT_ROOT_PATH}/common/ble_log/extension/log_compression/scripts/ble_log_compress.py)

        set(compressed_srcs "")
        set(uncompressed_srcs "")
        set(compressed_srcs_with_abs_path "")

        execute_process(COMMAND ${BLE_PYTHON_EXECUTABLE}
                                ${BT_ROOT_PATH}/common/ble_log/extension/log_compression/scripts/env_check.py
                            RESULT_VARIABLE result
                            OUTPUT_VARIABLE   out
                            ERROR_VARIABLE    err
                        )

        if(NOT ${result} EQUAL 0)
            message(WARNING "${err}")
            message(WARNING "Exit this log compression due to failure of environment check")
            set(LOG_COMPRESS_INCLUDE_DIRS ${include_dirs} PARENT_SCOPE)
            set(LOG_COMPRESSION_TARGET "" PARENT_SCOPE)
            return()
        endif()

        set(CONFIG_FILE_PATH "${CMAKE_BINARY_DIR}/ble_log/module_info.yml")
        if(NOT EXISTS "${CONFIG_FILE_PATH}")
            file(WRITE "${CMAKE_BINARY_DIR}/ble_log/module_info.yml")
        endif()
        set(YML_IN  "${BT_ROOT_PATH}/common/ble_log/extension/log_compression/scripts/configs/module_info.yml.in")
        configure_file(${YML_IN} ${CONFIG_FILE_PATH} @ONLY)

        string(REPLACE ";" "|" MODULE_CODE_PATH "${LOG_COMPRESSED_MODULE_CODE_PATH}")
        set(MATCH_PATTERN "(${MODULE_CODE_PATH}).+\\.c")
        foreach(src ${srcs})
            if(src MATCHES ${MATCH_PATTERN})
                set(dest "${LOG_COMPRESSED_SRCS_DIR}/${src}")
                file(WRITE "${dest}" "")
                list(APPEND compressed_srcs ${src})
                list(APPEND compressed_srcs_with_abs_path "${dest}")
            else()
                list(APPEND uncompressed_srcs ${src})
            endif()
        endforeach()
        string(REPLACE "|" ";" LOG_COMPRESSED_MODULE_CODE_PATH "${MODULE_CODE_PATH}")

        # Some header files of NIMBLE are not added to include_dirs,
        # but rely on relative path searches. This will cause the header
        # files to be found due to the change in the source code location
        # after using the log compression scheme.
        # Therefore, these paths are added to include_dirs here to avoid
        # unfinished compilation errors.
        if(CONFIG_BT_NIMBLE_ENABLED)
            list(APPEND include_dirs
                "host/nimble/nimble/nimble/host/src"
                "host/nimble/nimble/nimble/host/store/config/src")
        endif()

        add_custom_target(ble_log_compression ALL
            COMMAND ${BLE_PYTHON_EXECUTABLE} ${PYTHON_SCRIPT}
                compress
                --compressed_srcs_path "${LOG_COMPRESSED_SRCS_DIR}"
                --build_path "${CMAKE_BINARY_DIR}"
                --module "'${LOG_COMPRESSED_MODULE}'"
                --bt_path "${BT_ROOT_PATH}"
                --srcs "'${compressed_srcs}'"
            DEPENDS ${compressed_srcs_with_abs_path} ${PYTHON_SCRIPT}
            COMMENT "Log compression is being performed, please wait..."
            WORKING_DIRECTORY ${BT_ROOT_PATH}
            USES_TERMINAL
        )

        function(add_flags_if_in_list file file_list compile_flags)
            set(PROCESSED OFF PARENT_SCOPE)
            foreach(item IN LISTS file_list)
                if(item STREQUAL file)
                    set_source_files_properties("${LOG_COMPRESSED_SRCS_DIR}/${file}"
                        PROPERTIES
                            GENERATED TRUE
                            COMPILE_FLAGS "${compile_flags}"
                            OBJECT_DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/ble_log_compression"
                    )
                    set(PROCESSED ON PARENT_SCOPE)
                    break()
                endif()
            endforeach()
        endfunction()

        foreach(src ${compressed_srcs})
            set(PROCESSED OFF)
            if(CONFIG_BT_BLUEDROID_ENABLED)
                    set(files_with_compile_flags
                        "host/bluedroid/bta/gatt/bta_gattc_act.c"
                        "host/bluedroid/bta/gatt/bta_gattc_cache.c"
                        "host/bluedroid/btc/profile/std/gatt/btc_gatt_util.c"
                        "host/bluedroid/btc/profile/std/gatt/btc_gatts.c")
                add_flags_if_in_list("${src}"
                                     "${files_with_compile_flags}"
                                     "-Wno-address-of-packed-member")

                if(NOT CMAKE_BUILD_EARLY_EXPANSION)
                    set(jump_table_opts "-fjump-tables")
                    if(NOT (CMAKE_C_COMPILER_ID MATCHES "Clang") )
                        set(jump_table_opts "${jump_table_opts} -ftree-switch-conversion")
                    endif()
                    set(files_with_compile_flags
                        "host/bluedroid/bta/hf_ag/bta_ag_cmd.c"
                        "host/bluedroid/btc/profile/std/gap/btc_gap_ble.c"
                    )
                    add_flags_if_in_list("${src}"
                                         "${files_with_compile_flags}"
                                         "${jump_table_opts}")
                endif()

                if(CMAKE_C_COMPILER_ID MATCHES "GNU" AND CMAKE_C_COMPILER_VERSION VERSION_GREATER 15.0)
                    set(files_with_compile_flags "host/bluedroid/device/controller.c")
                    add_flags_if_in_list("${src}"
                                         "${files_with_compile_flags}"
                                         "-Wno-unterminated-string-initialization")
                endif()
            endif()
            if(CONFIG_BT_NIMBLE_ENABLED)
                if(CONFIG_BT_NIMBLE_MESH)
                    message(ERROR "The current log compression scheme does not support NIMBLE MESH")
                endif()
            endif()
            if(NOT PROCESSED)
                set_source_files_properties("${LOG_COMPRESSED_SRCS_DIR}/${src}"
                    PROPERTIES GENERATED TRUE
                    OBJECT_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/ble_log_compression
                )
            endif()
        endforeach()

        set(LOG_COMPRESSION_TARGET ble_log_compression PARENT_SCOPE)
        # set(LOG_COMPRESSION_TARGET "" PARENT_SCOPE)
        set(LOG_COMPRESS_SRCS "${compressed_srcs_with_abs_path};${uncompressed_srcs}" PARENT_SCOPE)
        list(APPEND include_dirs "common/ble_log/extension/log_compression/include")
        set(LOG_COMPRESS_INCLUDE_DIRS ${include_dirs} PARENT_SCOPE)
    else()
        set(LOG_COMPRESSION_TARGET "" PARENT_SCOPE)
        message("Python 3 used for log compressing not found")
    endif()
else()
    set(LOG_COMPRESSION_TARGET "" PARENT_SCOPE)
    message(STATUS "No module enabled for log compress")
endif()
