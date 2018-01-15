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
    if($ENV{${variable}})
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
