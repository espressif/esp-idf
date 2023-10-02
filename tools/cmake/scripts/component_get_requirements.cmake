# if(IN_LIST) is used, which requires CMP0057 set to NEW
cmake_policy(SET CMP0057 NEW)
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

#
# Given a component name or alias, get the corresponding component target.
#
function(__component_get_target var name_or_alias)
    idf_build_get_property(component_targets __COMPONENT_TARGETS)

    # Assume first that the paramters is an alias.
    string(REPLACE "::" "_" name_or_alias "${name_or_alias}")
    set(component_target ___${name_or_alias})

    if(component_target IN_LIST component_targets)
        set(${var} ${component_target} PARENT_SCOPE)
        set(target ${component_target})
    else() # assumption is wrong, try to look for it manually
        unset(target)
        foreach(component_target ${component_targets})
            __component_get_property(_component_name ${component_target} COMPONENT_NAME)
            if(name_or_alias STREQUAL _component_name)
                set(target ${component_target})
                break()
            endif()
        endforeach()
        set(${var} ${target} PARENT_SCOPE)
    endif()
endfunction()

function(idf_component_get_property var component property)
    __component_get_target(component_target ${component})
    __component_get_property(_var ${component_target} ${property})
    set(${var} ${_var} PARENT_SCOPE)
endfunction()

macro(require_idf_targets)
endmacro()

macro(idf_component_mock)
    set(options)
    set(single_value)
    set(multi_value MOCK_HEADER_FILES INCLUDE_DIRS REQUIRES)
    cmake_parse_arguments(_ "${options}" "${single_value}" "${multi_value}" "${ARGN}")
    idf_component_register(REQUIRES ${__REQUIRES} cmock)
    return()
endmacro()

macro(idf_component_register)
    set(options WHOLE_ARCHIVE)
    set(single_value KCONFIG KCONFIG_PROJBUILD)
    set(multi_value SRCS SRC_DIRS EXCLUDE_SRCS
                    INCLUDE_DIRS PRIV_INCLUDE_DIRS LDFRAGMENTS REQUIRES
                    PRIV_REQUIRES REQUIRED_IDF_TARGETS EMBED_FILES EMBED_TXTFILES)
    cmake_parse_arguments(_ "${options}" "${single_value}" "${multi_value}" "${ARGN}")
    set(__component_priv_requires "${__PRIV_REQUIRES}")
    set(__component_requires "${__REQUIRES}")
    set(__component_kconfig "${__KCONFIG}")
    set(__component_kconfig_projbuild "${__KCONFIG_PROJBUILD}")
    set(__component_include_dirs "${__INCLUDE_DIRS}")
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
    spaces2list(__component_include_dirs)

    set(__component_requires "${__component_requires}" PARENT_SCOPE)
    set(__component_priv_requires "${__component_priv_requires}" PARENT_SCOPE)
    set(__component_kconfig "${__component_kconfig}" PARENT_SCOPE)
    set(__component_kconfig_projbuild "${__component_kconfig_projbuild}" PARENT_SCOPE)
    set(__component_include_dirs "${__component_include_dirs}" PARENT_SCOPE)
    set(__component_registered ${__component_registered} PARENT_SCOPE)
endfunction()

set(CMAKE_BUILD_EARLY_EXPANSION 1)
foreach(__component_target ${__component_targets})
    set(__component_requires "")
    set(__component_priv_requires "")
    set(__component_registered 0)

    __component_get_requirements()

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
__component_set_property(${__component_target} __COMPONENT_REGISTERED ${__component_registered})
__component_set_property(${__component_target} INCLUDE_DIRS \"${__component_include_dirs}\")"
    )

    if(__component_kconfig)
        get_filename_component(__component_kconfig "${__component_kconfig}" ABSOLUTE)
        set(__contents
"${__contents}\n__component_set_property(${__component_target} KCONFIG \"${__component_kconfig}\")"
            )
    endif()

    if(__component_kconfig_projbuild)
        get_filename_component(__component_kconfig "${__component_kconfig}" ABSOLUTE)
        set(__contents
"${__contents}\n__component_set_property(${__component_target} KCONFIG_PROJBUILD \"${__component_kconfig_projbuild}\")"
            )
    endif()

    set(__component_requires_contents "${__component_requires_contents}\n${__contents}")
endforeach()

file(WRITE ${COMPONENT_REQUIRES_FILE} "${__component_requires_contents}")
