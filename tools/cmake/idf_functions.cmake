# Some IDF-specific functions and functions


# Add all the IDF global compiler & preprocessor options
# (applied to all components). Some are config-dependent
#
# If you only want to set options for a particular component,
# don't call or edit this function. TODO DESCRIBE WHAT TO DO INSTEAD
#
function(idf_set_global_compiler_options)
  add_definitions(-DESP_PLATFORM)
  add_definitions(-DHAVE_CONFIG_H)

  if(CONFIG_OPTIMIZATION_LEVEL_RELEASE)
    add_compile_options(-Os)
  else()
    add_compile_options(-Og)
  endif()

  add_c_compile_options(-std=gnu99)

  add_cxx_compile_options(-std=gnu++11 -fno-rtti)

  if(CONFIG_CXX_EXCEPTIONS)
    add_cxx_compile_options(-fexceptions)
  else()
    add_cxx_compile_options(-fno-exceptions)
  endif()

  # Default compiler configuration
  add_compile_options(-ffunction-sections -fdata-sections -fstrict-volatile-bitfields -mlongcalls -nostdlib)

  # Default warnings configuration
  add_compile_options(-Wall -Werror=all -Wno-error=unused-function -Wno-error=unused-but-set-variable -Wno-error=unused-variable -Wno-error=deprecated-declarations -Wextra -Wno-unused-parameter -Wno-sign-compare)
  add_c_compile_options(-Wno-old-style-declaration)

  # Stack protection
  if(NOT BOOTLOADER_BUILD)
    if(CONFIG_STACK_CHECK_NORM)
      add_compile_options(-fstack-protector)
    elseif(CONFIG_STACK_CHECK_STRONG)
      add_compile_options(-fstack-protector-strong)
    elseif(CONFIG_STACK_CHECK_ALL)
      add_compile_options(-fstack-protector-all)
    endif()
  endif()

  if(CONFIG_OPTIMIZATION_ASSERTIONS_DISABLED)
    add_definitions(-DNDEBUG)
  endif()

  # Always generate debug symbols (even in Release mode, these don't
  # go itno ther final binary
  add_compile_options(-ggdb)

endfunction()

# Override add_executable to add IDF-specific
# linker flags & map file to all built executables
function(add_executable target)
  get_filename_component(basename ${target} NAME_WE)
  set(mapfile "${basename}.map")

  _add_executable(${ARGV})

  target_link_libraries(${target} "-Wl,--gc-sections -Wl,--cref -Wl,--Map=${mapfile} -Wl,--start-group")
endfunction()
