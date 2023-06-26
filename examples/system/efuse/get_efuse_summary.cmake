cmake_minimum_required(VERSION 3.16)

include(${esptool_py_dir}/espefuse.cmake)
espefuse_get_json_summary(efuse_json)
foreach(name ${efuse_names})
    espefuse_get_efuse(ret_data "${efuse_json}" ${name} "value")
    message(STATUS "FROM_CMAKE: ${name}: ${ret_data}")
endforeach()
