#
# Convert a file (text or binary) into a C source file suitable
# for gcc. Designed to replicate 'objcopy' with more predictable
# naming, and supports appending a null byte for embedding text as
# a string.
#
# Designed to be run as a script with "cmake -P"
#
# Set variables DATA_FILE, SOURCE_FILE, FILE_TYPE when running this.
#
# If FILE_TYPE is set to STRING, a null byte is appended to DATA_FILE's contents
# before it is embedded.
#
# If FILE_TYPE is unset (or any other value), DATA_FILE is embedded
# verbatim.
#
#
if(NOT DATA_FILE)
  message(FATAL_ERROR "DATA_FILE for converting must be specified")
endif()

if(NOT SOURCE_FILE)
  message(FATAL_ERROR "SOURCE_FILE destination must be specified")
endif()

file(READ "${DATA_FILE}" data HEX)

if(FILE_TYPE STREQUAL "TEXT")
  set(data "${data}00")  # null-byte terimnation
endif()

## Convert string of raw hex bytes to lines of hex bytes in C array format
string(REGEX REPLACE "................................" "\\0\n    " data "${data}")  # 16 bytes per line
string(REGEX REPLACE "[0-9a-f][0-9a-f]" "0x\\0, " data "${data}")                    # hex formatted C bytes
string(REGEX REPLACE ", $" "" data "${data}")                                        # trim the last comma (cosmetic)

## Come up with C-friendly name for file
get_filename_component(source_filename "${DATA_FILE}" NAME)
string(MAKE_C_IDENTIFIER "${source_filename}" varname)

file(WRITE "${SOURCE_FILE}"  "/*\n")
file(APPEND "${SOURCE_FILE}" " * Data converted from ${DATA_FILE}\n")
if(FILE_TYPE STREQUAL "TEXT")
  file(APPEND "${SOURCE_FILE}" " * (null byte appended)\n")
endif()
file(APPEND "${SOURCE_FILE}" " */\n")
file(APPEND "${SOURCE_FILE}" "#include <stdlib.h>\n")
file(APPEND "${SOURCE_FILE}" "#include <stddef.h>\n\n")

file(APPEND "${SOURCE_FILE}" "const char ${varname}[] = {\n    ${data}\n    };\n")
file(APPEND "${SOURCE_FILE}" "const size_t ${varname}_length = sizeof(${varname});\n\n")

# Backwards compatibility, matches objcopy binary symbol names
file(APPEND "${SOURCE_FILE}" "/* Backwards compatible names, match objcopy -I binary naming */\n")
file(APPEND "${SOURCE_FILE}" "const char *_binary_${varname}_start = ${varname};\n")
file(APPEND "${SOURCE_FILE}" "const char *_binary_${varname}_end = ${varname} + sizeof(${varname});\n")
