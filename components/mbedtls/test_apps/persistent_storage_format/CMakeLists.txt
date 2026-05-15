# Persistent storage format stability tests for the ESP PSA opaque drivers.
#
# This is a separate test project from `mbedtls_ut` because it depends on a
# pre-flashed NVS fixture image, has its own partition layout (NVS is reserved
# at a known offset), and is run by a dedicated pytest entry point.
cmake_minimum_required(VERSION 3.22)

set(EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/tools/test_apps/components")
set(COMPONENTS main)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(persistent_storage_format_test)

# Hook the shared multi-driver NVS fixture into `idf.py flash` so every
# CI runner gets a pre-populated NVS partition without manual steps.
# The same .bin works across runners — each only references its driver's
# key id from the fixture.
set(NVS_FIXTURE
    "${CMAKE_CURRENT_SOURCE_DIR}/fixtures/nvs_efuse_v1.bin")
if(EXISTS ${NVS_FIXTURE})
    partition_table_get_partition_info(nvs_offset "--partition-name nvs" "offset")
    partition_table_get_partition_info(nvs_size   "--partition-name nvs" "size")
    if(NOT nvs_offset OR NOT nvs_size)
        message(FATAL_ERROR "persistent_storage_format: nvs partition not found "
                            "in partition table — fixture cannot be flashed.")
    endif()
    # NVS partition is plaintext-on-flash (IDF excludes it from auto flash
    # encryption). Force plaintext writes even when SECURE_FLASH_ENC is on,
    # otherwise the bytes get encrypted-on-write and NVS reads them as
    # garbage at runtime.
    esptool_py_flash_target_image(flash "nvs" ${nvs_offset} ${NVS_FIXTURE}
                                  ALWAYS_PLAINTEXT)
    message(STATUS "persistent_storage_format: will flash ${NVS_FIXTURE} "
                   "to nvs @ ${nvs_offset} (size ${nvs_size}).")
endif()
