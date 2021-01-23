idf_build_get_property(spi_flash_mock CONFIG_SPI_FLASH_MOCK)
idf_build_get_property(target IDF_TARGET)
if(${spi_flash_mock})
    message(STATUS "building SPI FLASH MOCKS")

    set(IDF_PATH $ENV{IDF_PATH})
    set(CMOCK_DIR "${IDF_PATH}/components/cmock/CMock")
    set(MOCK_GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/mocks")

    file(MAKE_DIRECTORY ${MOCK_GEN_DIR})

    set(MOCK_OUTPUT
        "${MOCK_GEN_DIR}/Mockesp_partition.c" "${MOCK_GEN_DIR}/Mockesp_partition.h"
        "${MOCK_GEN_DIR}/Mockesp_flash.c" "${MOCK_GEN_DIR}/Mockesp_flash.h"
        "${MOCK_GEN_DIR}/Mockesp_spi_flash.c" "${MOCK_GEN_DIR}/Mockesp_spi_flash.h")

    set(MOCK_HEADERS
        ${CMAKE_CURRENT_SOURCE_DIR}/include/esp_partition.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/esp_flash.h
        ${CMAKE_CURRENT_SOURCE_DIR}/include/esp_spi_flash.h
        )

    set(ENV{UNITY_DIR} "$ENV{IDF_PATH}/components/cmock/CMock")

    set(include_dirs
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${MOCK_GEN_DIR}")

    set(srcs "${MOCK_GEN_DIR}/Mockesp_partition.c"
             "${MOCK_GEN_DIR}/Mockesp_spi_flash.c"
             "${MOCK_GEN_DIR}/Mockesp_flash.c")

    if(${target} STREQUAL "linux")
        list(APPEND include_dirs
            "${CMAKE_CURRENT_SOURCE_DIR}/../spi_flash/sim/stubs/soc/include"
            "${CMAKE_CURRENT_SOURCE_DIR}/../spi_flash/sim/stubs/xtensa")
    endif()

    idf_component_register(SRCS "${srcs}"
                        INCLUDE_DIRS ${include_dirs}
                        REQUIRES cmock)

    add_custom_command(
        OUTPUT ruby_found SYMBOLIC
        COMMAND "ruby" "-v"
        COMMENT "Try to find ruby. If this fails, you need to install ruby"
    )

    # This command builds the mocks.
    # First, environment variable UNITY_DIR is set. This is necessary to prevent unity from looking in its own submodule
    # which doesn't work in our CI yet...
    # The rest is a straight forward call to cmock.rb, consult cmock's documentation for more information.
    add_custom_command(
        OUTPUT ${MOCK_OUTPUT}
        DEPENDS ruby_found
        COMMAND ${CMAKE_COMMAND} -E env "UNITY_DIR=${IDF_PATH}/components/unity/unity"
            ruby
            ${CMOCK_DIR}/lib/cmock.rb
            -o${CMAKE_CURRENT_SOURCE_DIR}/mock/mock_config.yaml
            ${MOCK_HEADERS}
      )

else()
    if(BOOTLOADER_BUILD)
        set(srcs "${target}/spi_flash_rom_patch.c")
        set(cache_srcs "")
        set(priv_requires bootloader_support soc)
    else()
        set(cache_srcs
            "cache_utils.c"
            "flash_mmap.c"
            "flash_ops.c"
            "${target}/flash_ops_${target}.c"
        )
        set(srcs
            "partition.c"
            "${target}/spi_flash_rom_patch.c"
        )

        # New implementation after IDF v4.0
        list(APPEND srcs
            "spi_flash_chip_drivers.c"
            "spi_flash_chip_generic.c"
            "spi_flash_chip_issi.c"
            "spi_flash_chip_mxic.c"
            "spi_flash_chip_gd.c"
            "spi_flash_chip_winbond.c"
            "spi_flash_chip_boya.c"
            "memspi_host_driver.c")

        list(APPEND cache_srcs
            "esp_flash_api.c"
            "esp_flash_spi_init.c"
            "spi_flash_os_func_app.c"
            "spi_flash_os_func_noos.c")

        list(APPEND srcs ${cache_srcs})
        set(priv_requires bootloader_support app_update soc esp_ipc)
    endif()

    idf_component_register(SRCS "${srcs}"
                        REQUIRES hal
                        PRIV_REQUIRES "${priv_requires}"
                        INCLUDE_DIRS include
                        PRIV_INCLUDE_DIRS private_include
                        LDFRAGMENTS linker.lf)

    # Avoid cache miss by unexpected inlineing when built by -Os
    set_source_files_properties(${cache_srcs} PROPERTIES COMPILE_FLAGS
        "-fno-inline-functions -fno-inline-small-functions -fno-inline-functions-called-once")

endif()
