# set_default
#
# Define a variable to a default value if otherwise unset.
#
# Priority for new value is:
# - Existing cmake value (ie set with cmake -D, or already set in CMakeLists)
# - Value of any non-empty environment variable of the same name
# - Default value as provided to function
#
function(set_default variable default_value)
    if(NOT ${variable})
        if(DEFINED ENV{${variable}} AND NOT "$ENV{${variable}}" STREQUAL "")
            set(${variable} $ENV{${variable}} PARENT_SCOPE)
        else()
            set(${variable} ${default_value} PARENT_SCOPE)
        endif()
    endif()
endfunction()

# spaces2list
#
# Take a variable whose value was space-delimited values, convert to a cmake
# list (semicolon-delimited)
#
# Note: if using this for directories, keeps the issue in place that
# directories can't contain spaces...
#
# TODO: look at cmake separate_arguments, which is quote-aware
function(spaces2list variable_name)
    string(REPLACE " " ";" tmp "${${variable_name}}")
    set("${variable_name}" "${tmp}" PARENT_SCOPE)
endfunction()

# lines2list
#
# Take a variable with multiple lines of output in it, convert it
# to a cmake list (semicolon-delimited), one line per item
#
function(lines2list variable_name)
    string(REGEX REPLACE "\r?\n" ";" tmp "${${variable_name}}")
    string(REGEX REPLACE ";;" ";" tmp "${tmp}")
    set("${variable_name}" "${tmp}" PARENT_SCOPE)
endfunction()


# move_if_different
#
# If 'source' has different md5sum to 'destination' (or destination
# does not exist, move it across.
#
# If 'source' has the same md5sum as 'destination', delete 'source'.
#
# Avoids timestamp updates for re-generated files where content hasn't
# changed.
function(move_if_different source destination)
    set(do_copy 1)
    file(GLOB dest_exists ${destination})
    if(dest_exists)
        file(MD5 ${source} source_md5)
        file(MD5 ${destination} dest_md5)
        if(source_md5 STREQUAL dest_md5)
            set(do_copy "")
        endif()
    endif()

    if(do_copy)
        message("Moving ${source} -> ${destination}")
        file(RENAME ${source} ${destination})
    else()
        message("Not moving ${source} -> ${destination}")
        file(REMOVE ${source})
    endif()

endfunction()

# target_add_binary_data adds binary data into the built target,
# by converting it to a generated source file which is then compiled
# to a binary object as part of the build
function(target_add_binary_data target embed_file embed_type)
    cmake_parse_arguments(_ "" "RENAME_TO" "" ${ARGN})
    idf_build_get_property(build_dir BUILD_DIR)
    idf_build_get_property(idf_path IDF_PATH)

    get_filename_component(embed_file "${embed_file}" ABSOLUTE)

    get_filename_component(name "${embed_file}" NAME)
    set(embed_srcfile "${build_dir}/${name}.S")

    set(rename_to_arg)
    if(__RENAME_TO)  # use a predefined variable name
        set(rename_to_arg -D "VARIABLE_BASENAME=${__RENAME_TO}")
    endif()

    add_custom_command(OUTPUT "${embed_srcfile}"
        COMMAND "${CMAKE_COMMAND}"
        -D "DATA_FILE=${embed_file}"
        -D "SOURCE_FILE=${embed_srcfile}"
        ${rename_to_arg}
        -D "FILE_TYPE=${embed_type}"
        -P "${idf_path}/tools/cmake/scripts/data_file_embed_asm.cmake"
        MAIN_DEPENDENCY "${embed_file}"
        DEPENDS "${idf_path}/tools/cmake/scripts/data_file_embed_asm.cmake"
        WORKING_DIRECTORY "${build_dir}"
        VERBATIM)

    set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES "${embed_srcfile}")

    target_sources("${target}" PRIVATE "${embed_srcfile}")
endfunction()

macro(include_if_exists path)
    if(EXISTS "${path}")
        include("${path}")
    endif()
endmacro()

# Append a single line to the file specified
# The line ending is determined by the host OS
function(file_append_line file line)
    if(DEFINED ENV{MSYSTEM} OR CMAKE_HOST_WIN32)
        set(line_ending "\r\n")
    else() # unix
        set(line_ending "\n")
    endif()
    file(READ ${file} existing)
    string(FIND ${existing} ${line_ending} last_newline REVERSE)
    string(LENGTH ${existing} length)
    math(EXPR length "${length}-1")
    if(NOT length EQUAL last_newline) # file doesn't end with a newline
        file(APPEND "${file}" "${line_ending}")
    endif()
    file(APPEND "${file}" "${line}${line_ending}")
endfunction()

# Add one or more linker scripts to the target, including a link-time dependency
#
# Automatically adds a -L search path for the containing directory (if found),
# and then adds -T with the filename only. This allows INCLUDE directives to be
# used to include other linker scripts in the same directory.
function(target_linker_script target deptype scriptfiles)
    cmake_parse_arguments(_ "" "PROCESS" "" ${ARGN})
    foreach(scriptfile ${scriptfiles})
        get_filename_component(abs_script "${scriptfile}" ABSOLUTE)
        message(STATUS "Adding linker script ${abs_script}")

        if(__PROCESS)
            get_filename_component(output "${__PROCESS}" ABSOLUTE)
            __ldgen_process_template(${abs_script} ${output})
            set(abs_script ${output})
        endif()

        get_filename_component(search_dir "${abs_script}" DIRECTORY)
        get_filename_component(scriptname "${abs_script}" NAME)

        if(deptype STREQUAL INTERFACE OR deptype STREQUAL PUBLIC)
            get_target_property(link_libraries "${target}" INTERFACE_LINK_LIBRARIES)
        else()
            get_target_property(link_libraries "${target}" LINK_LIBRARIES)
        endif()

        list(FIND "${link_libraries}" "-L ${search_dir}" found_search_dir)
        if(found_search_dir EQUAL "-1")  # not already added as a search path
            target_link_libraries("${target}" "${deptype}" "-L ${search_dir}")
        endif()

        target_link_libraries("${target}" "${deptype}" "-T ${scriptname}")

        # Note: In ESP-IDF, most targets are libraries and libary LINK_DEPENDS don't propagate to
        # executable(s) the library is linked to. Attach manually to executable once it is known.
        #
        # Property INTERFACE_LINK_DEPENDS is available in CMake 3.13 which should propagate link
        # dependencies.
        if(NOT __PROCESS)
            idf_build_set_property(__LINK_DEPENDS ${abs_script} APPEND)
        endif()
    endforeach()
endfunction()

# Convert a CMake list to a JSON list and store it in a variable
function(make_json_list list variable)
    string(REPLACE ";" "\", \"" result "[ \"${list}\" ]")
    set("${variable}" "${result}" PARENT_SCOPE)
endfunction()

# add_prefix
#
# Adds a prefix to each item in the specified list.
#
function(add_prefix var prefix)
    foreach(elm ${ARGN})
        list(APPEND newlist "${prefix}${elm}")
    endforeach()
    set(${var} "${newlist}" PARENT_SCOPE)
endfunction()

# fail_at_build_time
#
# Creates a phony target which fails the build and touches CMakeCache.txt to cause a cmake run next time.
#
# This is used when a missing file is required at CMake runtime, but we can't fail the build if it is not found,
# because the "menuconfig" target may be required to fix the problem.
#
# We cannot use CMAKE_CONFIGURE_DEPENDS instead because it only works for files which exist at CMake runtime.
#
function(fail_at_build_time target_name message_line0)
    idf_build_get_property(idf_path IDF_PATH)
    set(message_lines COMMAND ${CMAKE_COMMAND} -E echo "${message_line0}")
    foreach(message_line ${ARGN})
        set(message_lines ${message_lines} COMMAND ${CMAKE_COMMAND} -E echo "${message_line}")
    endforeach()
    # Generate a timestamp file that gets included. When deleted on build, this forces CMake
    # to rerun.
    string(RANDOM filename)
    set(filename "${CMAKE_CURRENT_BINARY_DIR}/${filename}.cmake")
    file(WRITE "${filename}" "")
    include("${filename}")
    add_custom_target(${target_name} ALL
        ${message_lines}
        COMMAND ${CMAKE_COMMAND} -E remove "${filename}"
        COMMAND ${CMAKE_COMMAND} -P ${idf_path}/tools/cmake/scripts/fail.cmake
        VERBATIM)
endfunction()

function(check_exclusive_args args prefix)
    set(_args ${args})
    spaces2list(_args)
    set(only_arg 0)
    foreach(arg ${_args})
        if(${prefix}_${arg} AND only_arg)
            message(FATAL_ERROR "${args} are exclusive arguments")
        endif()

        if(${prefix}_${arg})
            set(only_arg 1)
        endif()
    endforeach()
endfunction()


# add_compile_options variant for C++ code only
#
# This adds global options, set target properties for
# component-specific flags
function(add_cxx_compile_options)
    foreach(option ${ARGV})
        # note: the Visual Studio Generator doesn't support this...
        add_compile_options($<$<COMPILE_LANGUAGE:CXX>:${option}>)
    endforeach()
endfunction()

# add_compile_options variant for C code only
#
# This adds global options, set target properties for
# component-specific flags
function(add_c_compile_options)
    foreach(option ${ARGV})
        # note: the Visual Studio Generator doesn't support this...
        add_compile_options($<$<COMPILE_LANGUAGE:C>:${option}>)
    endforeach()
endfunction()
