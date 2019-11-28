# For users checking this example, ignore the following code. This is so that
# the prebuilt project is built automatically in ESP-IDF CI.
if("$ENV{CI}")
    execute_process(COMMAND ${IDFTOOL} build
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/prebuilt)
endif()
