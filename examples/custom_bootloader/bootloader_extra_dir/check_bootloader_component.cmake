# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0

file(READ "${PROJECT_DESCRIPTION}" project_description)
string(JSON component_count LENGTH "${project_description}" build_components)

set(build_components)
if(component_count GREATER 0)
    math(EXPR last_component_index "${component_count} - 1")
    foreach(component_index RANGE 0 ${last_component_index})
        string(JSON component_name GET "${project_description}" build_components ${component_index})
        if(NOT component_name STREQUAL "")
            list(APPEND build_components "${component_name}")
        endif()
    endforeach()
endif()

string(REPLACE "," ";" expected_components "${EXPECTED_COMPONENTS}")
foreach(expected_component ${expected_components})
    list(FIND build_components "${expected_component}" component_index)
    if(component_index EQUAL -1)
        message(FATAL_ERROR "${expected_component} was not included in the bootloader build")
    endif()
endforeach()

string(REPLACE "," ";" ignored_components "${IGNORED_COMPONENTS}")
foreach(ignored_component ${ignored_components})
    list(FIND build_components "${ignored_component}" component_index)
    if(NOT component_index EQUAL -1)
        message(FATAL_ERROR "${ignored_component} was included in the bootloader build")
    endif()
endforeach()
