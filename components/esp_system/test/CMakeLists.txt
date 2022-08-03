set(requires "unity"
             "test_utils"
             "driver"
             "esp_timer")

set(excludes "test_ipc_isr.c"
             "test_ipc_isr.S"
             "test_ipc.c")

if(IDF_TARGET STREQUAL "esp32" OR IDF_TARGET STREQUAL "esp32s3")
    # If the target is esp32 or esp32s3, we can compile the IPC
    # tests.
    set(excludes "")
    # Add a required dependency
    list(APPEND requires "cmock")
endif()

idf_component_register(SRC_DIRS .
                    EXCLUDE_SRCS "${excludes}"
                    PRIV_INCLUDE_DIRS .
                    PRIV_REQUIRES "${requires}")
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
