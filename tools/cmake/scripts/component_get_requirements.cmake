include("${BUILD_PROPERTIES_FILE}")
include("${COMPONENT_PROPERTIES_FILE}")

function(idf_build_get_property var property)
    cmake_parse_arguments(_ "GENERATOR_EXPRESSION" "" "" ${ARGN})
    if(__GENERATOR_EXPRESSION)
        message(FATAL_ERROR "Getting build property generator expression not
                supported before idf_component_register().")
    endif()
    set(${var} ${${property}} PARENT_SCOPE)
endfunction()

idf_build_get_property(idf_path IDF_PATH)
include(${idf_path}/tools/cmake/utilities.cmake)
include(${idf_path}/tools/cmake/version.cmake)

function(__component_get_property var component_target property)
    set(_property __component_${component_target}_${property})
    set(${var} ${${_property}} PARENT_SCOPE)
endfunction()

macro(require_idf_targets)
endmacro()

macro(idf_component_register)
    set(options)
    set(single_value)
    set(multi_value SRCS SRC_DIRS EXCLUDE_SRCS
                    INCLUDE_DIRS PRIV_INCLUDE_DIRS LDFRAGMENTS REQUIRES
                    PRIV_REQUIRES REQUIRED_IDF_TARGETS EMBED_FILES EMBED_TXTFILES)
    cmake_parse_arguments(_ "${options}" "${single_value}" "${multi_value}" "${ARGN}")
    set(__component_requires "${__REQUIRES}")
    set(__component_priv_requires "${__PRIV_REQUIRES}")
    set(__component_registered 1)
    return()
endmacro()

macro(register_component)
    set(__component_requires "${COMPONENT_REQUIRES}")
    set(__component_priv_requires "${COMPONENT_PRIV_REQUIRES}")
    set(__component_registered 1)
    return()
endmacro()

macro(register_config_only_component)
    register_component()
endmacro()

idf_build_get_property(__common_reqs __COMPONENT_REQUIRES_COMMON)
idf_build_get_property(__component_targets __COMPONENT_TARGETS)

function(__component_get_requirements)
    # This is in a function (separate variable context) so that variables declared
    # and set by the included CMakeLists.txt does not bleed into the next inclusion.
    # We are only interested in the public and private requirements of components
    __component_get_property(__component_dir ${__component_target} COMPONENT_DIR)
    __component_get_property(__component_name ${__component_target} COMPONENT_NAME)
    set(COMPONENT_NAME ${__component_name})
    set(COMPONENT_DIR ${__component_dir})
    set(COMPONENT_PATH ${__component_dir}) # for backward compatibility only, COMPONENT_DIR is preferred
    include(${__component_dir}/CMakeLists.txt OPTIONAL)

    spaces2list(__component_requires)
    spaces2list(__component_priv_requires)

    set(__component_requires "${__component_requires}" PARENT_SCOPE)
    set(__component_priv_requires "${__component_priv_requires}" PARENT_SCOPE)
    set(__component_registered ${__component_registered} PARENT_SCOPE)
endfunction()

set(CMAKE_BUILD_EARLY_EXPANSION 1)
foreach(__component_target ${__component_targets})
    set(__component_requires "")
    set(__component_priv_requires "")
    set(__component_registered 0)

    __component_get_requirements()

    list(APPEND __component_requires "${__common_reqs}")

    # Remove duplicates and the component itself from its requirements
    __component_get_property(__component_alias ${__component_target} COMPONENT_ALIAS)
    __component_get_property(__component_name ${__component_target} COMPONENT_NAME)

    # Prevent component from linking to itself.
    if(__component_requires)
        list(REMOVE_DUPLICATES __component_requires)
        list(REMOVE_ITEM __component_requires ${__component_alias} ${__component_name})
    endif()

    if(__component_priv_requires)
        list(REMOVE_DUPLICATES __component_priv_requires)
        list(REMOVE_ITEM __component_priv_requires ${__component_alias} ${__component_name})
    endif()

    set(__contents
"__component_set_property(${__component_target} REQUIRES \"${__component_requires}\")
__component_set_property(${__component_target} PRIV_REQUIRES \"${__component_priv_requires}\")
__component_set_property(${__component_target} __COMPONENT_REGISTERED ${__component_registered})"
    )
    set(__component_requires_contents "${__component_requires_contents}\n${__contents}")
endforeach()

file(WRITE ${COMPONENT_REQUIRES_FILE} "${__component_requires_contents}")