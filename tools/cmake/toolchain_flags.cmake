# Map option variable names to their corresponding file lists
set(_IDF_TOOLCHAIN_OPTION_MAPPINGS
    "COMPILE_OPTIONS|cflags:cxxflags:asmflags"
    "C_COMPILE_OPTIONS|cflags"
    "CXX_COMPILE_OPTIONS|cxxflags"
    "ASM_COMPILE_OPTIONS|asmflags"
    "LINK_OPTIONS|ldflags"
)

# Extract keywords from mappings (drop substring after "|")
set(_IDF_TOOLCHAIN_MULTI_VALUE_KEYWORDS "")
foreach(mapping ${_IDF_TOOLCHAIN_OPTION_MAPPINGS})
    string(REGEX REPLACE "\\|.*" "" keyword "${mapping}")
    list(APPEND _IDF_TOOLCHAIN_MULTI_VALUE_KEYWORDS "${keyword}")
endforeach()

# Helper macro to process toolchain flag options based on mappings
# Uses macro instead of function to access parent scope variables directly
# operation: "add" or "remove"
macro(_process_toolchain_flag_options)
    cmake_parse_arguments(PARSE_ARGV 0 "" "" "" "${_IDF_TOOLCHAIN_MULTI_VALUE_KEYWORDS}")

    if(NOT EXISTS "${IDF_TOOLCHAIN_BUILD_DIR}")
        message(FATAL_ERROR "Toolchain directory does not exist: ${IDF_TOOLCHAIN_BUILD_DIR}")
    endif()

    foreach(mapping ${_IDF_TOOLCHAIN_OPTION_MAPPINGS})
        string(REPLACE "|" ";" mapping_list ${mapping})
        # Extract option variable name and add underscore prefix
        list(GET mapping_list 0 option_var)
        set(option_var "_${option_var}")

        # Extract file list for this option variable
        list(GET mapping_list 1 files)
        string(REPLACE ":" ";" files ${files})

        # Process flags based on calling function: add or remove
        if("${CMAKE_CURRENT_FUNCTION}" MATCHES "add_flags" AND DEFINED ${option_var})
            _add_flags_to_files("${files}" "${${option_var}}")
        endif()
        if("${CMAKE_CURRENT_FUNCTION}" MATCHES "remove_flags" AND ${option_var})
            _remove_flags_from_files("${files}" "${${option_var}}")
        endif()
    endforeach()
endmacro()

# Helper function to remove flags matching patterns from specific files
function(_remove_flags_from_files files patterns)
    foreach(file ${files})
        set(file_path "${IDF_TOOLCHAIN_BUILD_DIR}/${file}")

        if(EXISTS "${file_path}")
            file(STRINGS "${file_path}" existing_lines)
        else()
            set(existing_lines "")
        endif()

        file(WRITE "${file_path}" "")
        foreach(line ${existing_lines})
            set(should_keep TRUE)
            foreach(pattern ${patterns})
                if("${line}" MATCHES "${pattern}")
                    set(should_keep FALSE)
                    break()
                endif()
            endforeach()
            if(should_keep)
                file(APPEND "${file_path}" "${line}\n")
            endif()
        endforeach()
    endforeach()
endfunction()

# Helper function to add flags to specific files
function(_add_flags_to_files files flags)
    foreach(file ${files})
        set(file_path "${IDF_TOOLCHAIN_BUILD_DIR}/${file}")

        if(EXISTS "${file_path}")
            file(STRINGS "${file_path}" existing_lines)
        else()
            set(existing_lines "")
        endif()

        # Split flags string using regex to respect quotes.
        # Matches: double-quoted strings, single-quoted strings, or unquoted space-delimited tokens.
        #
        # TODO IDF-14467:
        #       Currently, paired linker flags are not properly handled when filtering.
        #       The regex-based splitting treats each flag independently, which breaks
        #       flag pairs that must be kept together. Future enhancement should support
        #       the following paired flag constructs:
        #         - --start-group ... --end-group
        #         - --push-state ... --pop-state
        #         - --whole-archive ... --no-whole-archive
        #         - --as-needed ... --no-as-needed
        #         - --push-section ... --pop-section
        string(REGEX MATCHALL "([^\\s\"']*(\"[^\"]*\"|'[^']*')|\"[^\"]*\"|'[^']*'|[^ \"']+)" flags "${flags}")

        file(WRITE "${file_path}" "")
        foreach(line ${existing_lines})
            set(should_keep TRUE)
            foreach(flag ${flags})
                if("${line}" STREQUAL "${flag}")
                    set(should_keep FALSE)
                    break()
                endif()
            endforeach()
            if(should_keep)
                file(APPEND "${file_path}" "${line}\n")
            endif()
        endforeach()

        foreach(flag ${flags})
            # Skip flags that contain IDF_TOOLCHAIN_BUILD_DIR substring
            # to avoid recursion
            string(FIND "${flag}" "${IDF_TOOLCHAIN_BUILD_DIR}" found_pos)
            if(found_pos EQUAL -1)
                file(APPEND "${file_path}" "${flag}\n")
            endif()
        endforeach()
    endforeach()
endfunction()

# idf_toolchain_add_flags
#
# @brief Add compiler or linker flags to the toolchain configuration files.
#
# This function adds flags to the appropriate toolchain response files (cflags, cxxflags,
# asmflags, or ldflags) in the IDF_TOOLCHAIN_BUILD_DIR directory. Duplicate flags
# are automatically removed before adding new ones.
#
# @param[in, optional] COMPILE_OPTIONS (multivalue) flags to add to C, C++, and ASM
#                     compilation. Applied to cflags, cxxflags, and asmflags files.
# @param[in, optional] C_COMPILE_OPTIONS (multivalue) flags to add to C compilation only.
#                     Applied to cflags file.
# @param[in, optional] CXX_COMPILE_OPTIONS (multivalue) flags to add to C++ compilation only.
#                     Applied to cxxflags file.
# @param[in, optional] ASM_COMPILE_OPTIONS (multivalue) flags to add to ASM compilation only.
#                     Applied to asmflags file.
# @param[in, optional] LINK_OPTIONS (multivalue) flags to add to linking.
#                     Applied to ldflags file.
#
# @note Multiple keyword arguments can be specified in a single call.
# @note Flags are deduplicated - if a flag already exists in the file, it will be
#       removed before adding the new one to ensure no duplicates.
#
# Example:
#   idf_toolchain_add_flags(COMPILE_OPTIONS "-Wall" "-Wextra")
#   idf_toolchain_add_flags(C_COMPILE_OPTIONS "-std=c99" LINK_OPTIONS "-Wl,--gc-sections")
function(idf_toolchain_add_flags)
    _process_toolchain_flag_options()
endfunction()

function(idf_toolchain_remove_flags)
    _process_toolchain_flag_options()
endfunction()

# Workaround: Re-run CMake compiler ABI detection after ABI flags are set.
#
# Problem: CMake performs compiler checks at an early stage during
# toolchain.cmake processing. At this early stage, response files are not yet
# ready, which causes CMake paths (e.g., CMAKE_<lang>_IMPLICIT_LINK_DIRECTORIES)
# to be incorrectly determined.
#
# Solution: Re-run the ABI detection after ABI flags are present to correctly
# determine these paths.
#
# Note: If the CMake API changes, this solution may need to be revised.
macro(idf_toolchain_rerun_abi_detection)
    set(lang_ext_pairs "C|c" "CXX|cpp")
    include(${CMAKE_ROOT}/Modules/CMakeDetermineCompilerABI.cmake)
    foreach(lang_ext ${lang_ext_pairs})
        string(REPLACE "|" ";" lang_ext_parts ${lang_ext})
        list(GET lang_ext_parts 0 lang)
        list(GET lang_ext_parts 1 ext)
        if(DEFINED CMAKE_${lang}_ABI_COMPILED)
            unset(CMAKE_${lang}_ABI_COMPILED)
            cmake_determine_compiler_abi(${lang} ${CMAKE_ROOT}/Modules/CMake${lang}CompilerABI.${ext})
        endif()
    endforeach()
endmacro()
