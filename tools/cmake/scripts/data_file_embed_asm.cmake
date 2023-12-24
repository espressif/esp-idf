#
# Convert a file (text or binary) into an assembler source file suitable
# for gcc. Designed to replicate 'objcopy' with more predictable
# naming, and supports appending a null byte for embedding text as
# a string.
#
# Designed to be run as a script with "cmake -P"
#
# Set variables DATA_FILE, SOURCE_FILE, FILE_TYPE when running this.
#
# If FILE_TYPE is set to TEXT, a null byte is appended to DATA_FILE's contents
# before SOURCE_FILE is created.
#
# If FILE_TYPE is unset (or any other value), DATA_FILE is copied
# verbatim into SOURCE_FILE.
#
#
if(NOT DATA_FILE)
    message(FATAL_ERROR "DATA_FILE for converting must be specified")
endif()

if(NOT SOURCE_FILE)
    message(FATAL_ERROR "SOURCE_FILE destination must be specified")
endif()

file(READ "${DATA_FILE}" data HEX)

string(LENGTH "${data}" data_len)
math(EXPR data_len "${data_len} / 2")  # 2 hex bytes per byte

if(FILE_TYPE STREQUAL "TEXT")
    set(data "${data}00")  # null-byte termination
endif()

## Convert string of raw hex bytes to lines of hex bytes as gcc .byte expressions
string(REGEX REPLACE "................................" ".byte \\0\n" data "${data}")  # 16 bytes per line
string(REGEX REPLACE "[^\n]+$" ".byte \\0\n" data "${data}")                           # last line
string(REGEX REPLACE "[0-9a-f][0-9a-f]" "0x\\0, " data "${data}")                      # hex formatted C bytes
string(REGEX REPLACE ", \n" "\n" data "${data}")                                       # trim the last comma

# Use source file name as variable name unless VARIABLE_BASENAME is set
if(NOT VARIABLE_BASENAME)
    get_filename_component(varname "${DATA_FILE}" NAME)
else()
    set(varname "${VARIABLE_BASENAME}")
endif()

function(append str)
    file(APPEND "${SOURCE_FILE}" "${str}")
endfunction()

function(append_line str)
    append("${str}\n")
endfunction()

function(make_and_append_identifier str)
    string(MAKE_C_IDENTIFIER "${str}" symbol)
    append_line("\n.global ${symbol}")
    append("${symbol}:")
    if(${ARGC} GREATER 1) # optional comment
        append(" /* ${ARGV1} */")
    endif()
    append("\n")
endfunction()

file(WRITE "${SOURCE_FILE}" "/*")
append_line(" * Data converted from ${DATA_FILE}")
if(FILE_TYPE STREQUAL "TEXT")
    append_line(" * (null byte appended)")
endif()
append_line(" */")

append_line(".data")
append_line("#if !defined (__APPLE__) && !defined (__linux__)")
append_line(".section .rodata.embedded")
append_line("#endif")
make_and_append_identifier("${varname}")
make_and_append_identifier("_binary_${varname}_start" "for objcopy compatibility")
append("${data}")
make_and_append_identifier("_binary_${varname}_end" "for objcopy compatibility")

append_line("")
if(FILE_TYPE STREQUAL "TEXT")
    make_and_append_identifier("${varname}_length" "not including null byte")
else()
    make_and_append_identifier("${varname}_length")
endif()
append_line(".long ${data_len}")
