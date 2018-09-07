find_package(Git)

if(NOT GIT_FOUND)
    message(WARNING "Git executable was not found. Git submodule checks will not be executed. "
        "If you have any build issues at all, start by adding git executable to the PATH and "
        "rerun cmake to not see this warning again.")

    function(git_submodule_check root_path)
        # no-op
    endfunction()
else()

    function(git_submodule_check root_path)

        execute_process(
            COMMAND ${GIT_EXECUTABLE} submodule status
            WORKING_DIRECTORY ${root_path}
            OUTPUT_VARIABLE submodule_status
            )

        # git submodule status output not guaranteed to be stable,
        # may need to check GIT_VERSION_STRING and do some fiddling in the
        # future...

        lines2list(submodule_status)

        foreach(line ${submodule_status})
            string(REGEX MATCH "(.)[0-9a-f]+ ([^\( ]+) ?" _ignored "${line}")
            set(status "${CMAKE_MATCH_1}")
            set(submodule_path "${CMAKE_MATCH_2}")

            if("${status}" STREQUAL "-")  # missing submodule
                message(STATUS "Initialising new submodule ${submodule_path}...")
                execute_process(
                    COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive ${submodule_path}
                    WORKING_DIRECTORY ${root_path}
                    RESULT_VARIABLE git_result
                    )
                if(git_result)
                    message(FATAL_ERROR "Git submodule init failed for ${submodule_path}")
                endif()

            elseif(NOT "${status}" STREQUAL " ")
                message(WARNING "Git submodule ${submodule_path} is out of date. "
                    "Run 'git submodule update --init --recursive' to fix.")
            endif()

            # Force a re-run of cmake if the submodule's .git file changes or is changed (ie accidental deinit)
            get_filename_component(submodule_abs_path ${submodule_path} ABSOLUTE BASE_DIR ${root_path})
            set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${submodule_abs_path}/.git)
            # same if the HEAD file in the submodule's directory changes (ie commit changes).
            # This will at least print the 'out of date' warning
            set(submodule_head "${root_path}/.git/modules/${submodule_path}/HEAD")
            if(EXISTS "${submodule_head}")
                set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${submodule_head})
            endif()

        endforeach()
    endfunction()

endif()
