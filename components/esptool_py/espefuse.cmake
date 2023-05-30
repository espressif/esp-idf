cmake_minimum_required(VERSION 3.16)

# Executes a espefuse.py command and returns a cleaned log
function(espefuse_cmd cmd output_log)
    set(SERIAL_TOOL ${ESPEFUSEPY})
    if(${ESPEFUSEPY_OFFLINE})
        set(VIRT_OPTION "--virt")
    endif()
    set(SERIAL_TOOL_ARGS ${VIRT_OPTION} "--chip;${IDF_TARGET};${cmd}")
    set(SERIAL_TOOL_SILENT 1)
    include(${esptool_py_dir}/run_serial_tool.cmake)
    set(log ${SERIAL_TOOL_OUTPUT_LOG})

    set(prefix_str " command ===")
    string(FIND "${log}" ${prefix_str} pos)
    if(${pos} GREATER -1)
        string(LENGTH ${prefix_str} len_of_prefix_str)
        math(EXPR pos "${pos} + ${len_of_prefix_str}")
        string(SUBSTRING "${log}" ${pos} -1 final_log)
    else()
        set(final_log "${log}")
    endif()

    set(${output_log} "${final_log}" PARENT_SCOPE)
endfunction()

# Reads efuses "espefuse.py summary" and returns JSON string
function(espefuse_get_json_summary json_str)
    espefuse_cmd("summary;--format;json" output_log)
    set(${json_str} "${output_log}" PARENT_SCOPE)
endfunction()

# See the esp-idf/docs/en/api-reference/system/efuse.rst "Get eFuses During Build".
#
# It takes the efuse json string and returns a value of property for a given efuse name
function(espefuse_get_efuse result efuse_json efuse_name efuse_property)
    if(${CMAKE_VERSION} VERSION_LESS "3.19.0")
        string(REGEX MATCH "\"${efuse_name}\":[ \t\n\r]*\{[^\}]*\}" cur_efuse "${efuse_json}")
        string(REGEX MATCH "\"${efuse_property}\":[ \t\n\r]*\"?([^,\"]*)\"?," ret_value "${cur_efuse}")
        set(${result} ${CMAKE_MATCH_1} PARENT_SCOPE)
    else()
        # The JSON feature has been supported by Cmake since 3.19.0
        string(JSON cur_efuse GET "${efuse_json}" ${efuse_name})
        string(JSON ret_value GET "${cur_efuse}" ${efuse_property})
        set(${result} ${ret_value} PARENT_SCOPE)
    endif()
endfunction()
