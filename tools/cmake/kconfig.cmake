include(ExternalProject)

macro(kconfig_set_variables)
  set(MCONF kconfig_bin/mconf)

  set_default(SDKCONFIG ${PROJECT_PATH}/sdkconfig)
  set(SDKCONFIG_HEADER ${CMAKE_BINARY_DIR}/sdkconfig.h)
  set(SDKCONFIG_CMAKE ${CMAKE_BINARY_DIR}/sdkconfig.cmake)

  set(ROOT_KCONFIG ${IDF_PATH}/Kconfig)
endmacro()

# Use the existing Makefile to build mconf (out of tree) when needed
#
# TODO: Find a solution on Windows
ExternalProject_Add(mconf
  SOURCE_DIR ${IDF_PATH}/tools/kconfig
  CONFIGURE_COMMAND ""
  BINARY_DIR "kconfig_bin"
  BUILD_COMMAND make -f ${IDF_PATH}/tools/kconfig/Makefile mconf
  BUILD_BYPRODUCTS "kconfig_bin" ${MCONF}
  INSTALL_COMMAND ""
  EXCLUDE_FROM_ALL 1
  )


# Find all Kconfig files for all components
function(kconfig_process_config)
  file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/include/config")
  set(kconfigs )
  set(kconfigs_projbuild )

  # Find Kconfig and Kconfig.projbuild for each component as applicable
  # if any of these change, cmake should rerun
  foreach(dir ${COMPONENT_PATHS} "${CMAKE_SOURCE_DIR}/main")
    file(GLOB kconfig "${dir}/Kconfig")
    if(kconfig)
      set(kconfigs "${kconfigs} ${kconfig}")
      set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${kconfig})
    endif()
    file(GLOB kconfig ${dir}/Kconfig.projbuild)
    if(kconfig)
      set(kconfigs_projbuild "${kconfigs_projbuild} ${kconfig}")
      set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${kconfig})
    endif()
  endforeach(dir ${COMPONENT_PATHS})

  if(EXISTS ${SDKCONFIG}.defaults)
    set(defaults_arg --defaults "${SDKCONFIG}.defaults")
  endif()

  set(confgen_basecommand
    ${PYTHON} ${IDF_PATH}/tools/kconfig_new/confgen.py
    --kconfig ${ROOT_KCONFIG}
    --config ${SDKCONFIG}
    ${defaults_arg}
    --create-config-if-missing
    --env "COMPONENT_KCONFIGS=${kconfigs}"
    --env "COMPONENT_KCONFIGS_PROJBUILD=${kconfigs_projbuild}")

  # Generate the menuconfig target (uses C-based mconf tool)
  add_custom_target(menuconfig
    DEPENDS mconf
    COMMAND ${confgen_basecommand} --output config ${SDKCONFIG}  # create any missing config file, with defaults if necessary
    COMMAND ${CMAKE_COMMAND} -E env
                "COMPONENT_KCONFIGS=${kconfigs}"
                "COMPONENT_KCONFIGS_PROJBUILD=${kconfigs_projbuild}"
                "KCONFIG_CONFIG=${SDKCONFIG}"
                ${MCONF} ${ROOT_KCONFIG}
    VERBATIM
    USES_TERMINAL)

  # Generate configuration output via confgen.py
  # makes sdkconfig.h and skdconfig.cmake
  #
  # This happens at cmake runtime not during the build
  execute_process(COMMAND ${confgen_basecommand}
    --output header ${SDKCONFIG_HEADER}
    --output cmake ${SDKCONFIG_CMAKE})

  # When sdkconfig file changes in the future, trigger a cmake run
  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${SDKCONFIG})

  # Ditto if either of the generated files are missing/modified (this is a bit irritating as it means
  # you can't edit these manually without them being regenerated, but I don't know of a better way...)
  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${SDKCONFIG_HEADER})
  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${SDKCONFIG_CMAKE})

  # Or if the config generation tool changes
  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${IDF_PATH}/tools/kconfig_new/confgen.py)
  set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${IDF_PATH}/tools/kconfig_new/kconfiglib.py)

endfunction()
