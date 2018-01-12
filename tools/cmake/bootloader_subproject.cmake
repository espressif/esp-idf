# Glue to build the bootloader subproject binary as an external
# cmake project under this one
#
#
ExternalProject_Add(bootloader_subproject
  # TODO: support overriding the bootloader in COMPONENT_PATHS
  SOURCE_DIR ${IDF_PATH}/components/bootloader/subproject
  BINARY_DIR ${CMAKE_BINARY_DIR}/bootloader_subproject
  CMAKE_ARGS -DSDKCONFIG=${SDKCONFIG} -DIDF_PATH=${IDF_PATH}
  INSTALL_COMMAND ""
  )


