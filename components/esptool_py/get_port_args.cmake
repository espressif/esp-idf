# Note: we can't expand these environment variables in the main IDF CMake build,
# because we want to expand them at flashing time not at CMake runtime (so they can change
# without needing a CMake re-run)
set(ESPPORT $ENV{ESPPORT})
if(NOT ESPPORT)
    message("Note: ${TOOL} will search for a serial port. To specify a port, set the ESPPORT environment variable.")
else()
    set(port_arg "-p ${ESPPORT}")
endif()

set(ESPBAUD $ENV{ESPBAUD})
if(NOT ESPBAUD)
    message("Note: ${TOOL} will attempt to set baud rate automatically. "
            "To specify a baud rate, set the ESPBAUD environment variable.")
else()
    set(baud_arg "-b ${ESPBAUD}")
endif()

