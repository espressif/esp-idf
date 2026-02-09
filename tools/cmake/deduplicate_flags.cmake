# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

# Remove duplicates from a string containing compilation flags
function(remove_duplicated_flags FLAGS UNIQFLAGS)
    set(FLAGS_LIST "${FLAGS}")
    # Convert the given flags, as a string, into a CMake list type
    separate_arguments(FLAGS_LIST)
    # Remove all the duplicated flags
    list(REMOVE_DUPLICATES FLAGS_LIST)
    # Convert the list back to a string
    string(REPLACE ";" " " FLAGS_LIST "${FLAGS_LIST}")
    # Return that string to the caller
    set(${UNIQFLAGS} "${FLAGS_LIST}" PARENT_SCOPE)
endfunction()
