# For users checking this example, ignore the following code. This is so that
# the prebuilt project is built automatically in ESP-IDF CI.
if("$ENV{CI}")
    # otherwise these file won't be rebuilt when switching the built target within the same job
    file(REMOVE
                ${CMAKE_SOURCE_DIR}/prebuilt/sdkconfig
                ${CMAKE_SOURCE_DIR}/main/libprebuilt.a
                ${CMAKE_SOURCE_DIR}/main/prebuilt.h
        )
    file(REMOVE_RECURSE ${CMAKE_SOURCE_DIR}/prebuilt/build)

    execute_process(COMMAND ${IDFTOOL} build
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/prebuilt)
endif()
