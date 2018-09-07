if(BOOTLOADER_BUILD)
    return()  # don't keep recursing!
endif()

# Glue to build the bootloader subproject binary as an external
# cmake project under this one
#
#
set(bootloader_build_dir "${CMAKE_BINARY_DIR}/bootloader")
set(bootloader_binary_files
    "${bootloader_build_dir}/bootloader.elf"
    "${bootloader_build_dir}/bootloader.bin"
    "${bootloader_build_dir}/bootloader.map"
    )

externalproject_add(bootloader
    # TODO: support overriding the bootloader in COMPONENT_PATHS
    SOURCE_DIR "${IDF_PATH}/components/bootloader/subproject"
    BINARY_DIR "${bootloader_build_dir}"
    CMAKE_ARGS -DSDKCONFIG=${SDKCONFIG} -DIDF_PATH=${IDF_PATH}
    INSTALL_COMMAND ""
    BUILD_ALWAYS 1  # no easy way around this...
    BUILD_BYPRODUCTS ${bootloader_binary_files}
    )

# this is a hack due to an (annoying) shortcoming in cmake, it can't
# extend the 'clean' target to the external project
# see thread: https://cmake.org/pipermail/cmake/2016-December/064660.html
#
# So for now we just have the top-level build remove the final build products...
set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" APPEND PROPERTY
    ADDITIONAL_MAKE_CLEAN_FILES
    ${bootloader_binary_files})
