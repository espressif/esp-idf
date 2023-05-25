# - Returns a version string from Git
#
# These functions force a re-configure on each git commit so that you can
# trust the values of the variables in your build system.
#
#  get_git_head_revision(<refspecvar> <hashvar> <repo dir> [<additional arguments to git describe> ...])
#
# Returns the refspec and sha hash of the current head revision
#
#  git_describe(<var> <repo dir> [<additional arguments to git describe> ...])
#
# Returns the results of git describe on the source tree, and adjusting
# the output so that it tests false if an error occurs.
#
#  git_get_exact_tag(<var> <repo dir> [<additional arguments to git describe> ...])
#
# Returns the results of git describe --exact-match on the source tree,
# and adjusting the output so that it tests false if there was no exact
# matching tag.
#
# Requires CMake 2.6 or newer (uses the 'function' command)
#
# Original Author:
# 2009-2010 Ryan Pavlik <rpavlik@iastate.edu> <abiryan@ryand.net>
# http://academic.cleardefinition.com
# Iowa State University HCI Graduate Program/VRAC
#
# Copyright Iowa State University 2009-2010.
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# http://www.boost.org/LICENSE_1_0.txt)
#
# Updated 2018 Espressif Systems to add _repo_dir argument
# to get revision of other repositories

if(__get_git_revision_description)
    return()
endif()
set(__get_git_revision_description YES)

# We must run the following at "include" time, not at function call time,
# to find the path to this module rather than the path to a calling list file
get_filename_component(_gitdescmoddir ${CMAKE_CURRENT_LIST_FILE} PATH)

function(get_git_head_revision _refspecvar _hashvar _repo_dir)
    execute_process(COMMAND
        "${GIT_EXECUTABLE}"
        rev-parse
        --git-dir
        WORKING_DIRECTORY
        ${_repo_dir}
        RESULT_VARIABLE
        res
        OUTPUT_VARIABLE
        GIT_DIR
        ERROR_VARIABLE
        error
        OUTPUT_STRIP_TRAILING_WHITESPACE)

    if(NOT res EQUAL 0)
        string(STRIP "${error}" error)
        message(STATUS "git rev-parse returned '${error}'")
        return()
    endif()

    get_filename_component(GIT_DIR "${GIT_DIR}" ABSOLUTE BASE_DIR "${_repo_dir}")

    set(GIT_DATA "${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/git-data")
    if(NOT EXISTS "${GIT_DATA}")
        file(MAKE_DIRECTORY "${GIT_DATA}")
    endif()

    if(NOT EXISTS "${GIT_DIR}/HEAD")
        return()
    endif()

    set(HEAD_FILE "${GIT_DATA}/HEAD")
    configure_file("${GIT_DIR}/HEAD" "${HEAD_FILE}" COPYONLY)

    configure_file("${_gitdescmoddir}/GetGitRevisionDescription.cmake.in"
        "${GIT_DATA}/grabRef.cmake"
        @ONLY)
    include("${GIT_DATA}/grabRef.cmake")

    set(${_refspecvar} "${HEAD_REF}" PARENT_SCOPE)
    set(${_hashvar} "${HEAD_HASH}" PARENT_SCOPE)
endfunction()

function(git_describe _var _repo_dir)
    if(NOT GIT_FOUND)
        find_package(Git QUIET)
    endif()
    get_git_head_revision(refspec hash "${_repo_dir}")
    if(NOT GIT_FOUND)
        set(${_var} "GIT-NOTFOUND" PARENT_SCOPE)
        return()
    endif()
    if(NOT hash)
        set(${_var} "HEAD-HASH-NOTFOUND" PARENT_SCOPE)
        return()

    endif()

    # TODO sanitize
    #if((${ARGN}" MATCHES "&&") OR
    #	(ARGN MATCHES "||") OR
    #	(ARGN MATCHES "\\;"))
    #	message("Please report the following error to the project!")
    #	message(FATAL_ERROR "Looks like someone's doing something nefarious with git_describe! Passed arguments ${ARGN}")
    #endif()

    #message(STATUS "Arguments to execute_process: ${ARGN}")

    execute_process(COMMAND
        "${GIT_EXECUTABLE}"
        "-C"
        ${_repo_dir}
        describe
        "--always"
        "--tags"
        "--dirty"
        ${ARGN}
        WORKING_DIRECTORY
        "${CMAKE_CURRENT_SOURCE_DIR}"
        RESULT_VARIABLE
        res
        OUTPUT_VARIABLE
        out
        ERROR_VARIABLE
        error
        OUTPUT_STRIP_TRAILING_WHITESPACE)
    if(NOT res EQUAL 0)
        string(STRIP "${error}" error)
        message(STATUS "git describe returned '${error}'")
        set(out "${out}-${res}-NOTFOUND")
    endif()

    set(${_var} "${out}" PARENT_SCOPE)
endfunction()

function(git_get_exact_tag _var _repo_dir)
    git_describe(out "${_repo_dir}" --exact-match ${ARGN})
    set(${_var} "${out}" PARENT_SCOPE)
endfunction()
