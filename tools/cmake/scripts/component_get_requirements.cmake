include(${IDF_PATH}/tools/cmake/utilities.cmake)

include("${BUILD_PROPERTIES_FILE}")
include("${SDKCONFIG_CMAKE}")

macro(require_idf_targets)
endmacro()

function(idf_build_get_property var property)
    cmake_parse_arguments(_ "GENERATOR_EXPRESSION" "" "" ${ARGN})
    if(__GENERATOR_EXPRESSION)
        message(FATAL_ERROR "Getting build property generator expression not
                supported before idf_component_register().")
    endif()
    set(${var} ${property} PARENT_SCOPE)
endfunction()

function(print_requires requires priv_requires)
    spaces2list(requires)
    spaces2list(priv_requires)
    string(REPLACE ";" ":" requires "${requires}")
    string(REPLACE ";" ":" priv_requires "${priv_requires}")
    message("${requires}:::${priv_requires}")
endfunction()

macro(idf_component_register)
    set(options)
    set(single_value)
    set(multi_value SRCS SRC_DIRS EXCLUDE_SRCS
                    INCLUDE_DIRS PRIV_INCLUDE_DIRS LDFRAGMENTS REQUIRES
                    PRIV_REQUIRES REQUIRED_IDF_TARGETS EMBED_FILES EMBED_TXTFILES)
    cmake_parse_arguments(_ "${options}" "${single_value}" "${multi_value}" "${ARGN}")
    print_requires("${__REQUIRES}" "${__PRIV_REQUIRES}")
    set(__is_component 1)
    return()
endmacro()

macro(register_component)
    print_requires("${COMPONENT_REQUIRES}" "${COMPONENT_PRIV_REQUIRES}")
    set(__is_component 1)
    return()
endmacro()

macro(register_config_only_component)
    register_component()
endmacro()

set(CMAKE_BUILD_EARLY_EXPANSION)
include(${COMPONENT_DIR}/CMakeLists.txt OPTIONAL)
