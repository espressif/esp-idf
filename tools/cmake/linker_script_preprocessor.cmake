# Split CFLAGS string into list for proper argument passing
# Uses UNIX_COMMAND method because NATIVE_COMMAND has issues on Windows
# where the first argument is parsed specially, causing problems with quoted paths
# Note: Paths are expected to be quoted in the CFLAGS variable
separate_arguments(CFLAGS_LIST UNIX_COMMAND "${CFLAGS}")

execute_process(COMMAND "${CC}" "-C" "-P" "-x" "c" "-E" ${CFLAGS_LIST} "${SOURCE}"
                RESULT_VARIABLE RET_CODE
                OUTPUT_VARIABLE PREPROCESSED_LINKER_SCRIPT
                ERROR_VARIABLE ERROR_VAR)
if(RET_CODE AND NOT RET_CODE EQUAL 0)
    message(FATAL_ERROR "Can't generate ${TARGET}\nRET_CODE: ${RET_CODE}\nERROR_MESSAGE: ${ERROR_VAR}")
endif()
string(REPLACE "\\n" "\n" TEXT "${PREPROCESSED_LINKER_SCRIPT}")
file(WRITE "${TARGET}" "${TEXT}")
