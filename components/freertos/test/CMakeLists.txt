# For refactored FreeRTOS unit tests, we need to support #include "xxx.h" of FreeRTOS headers
idf_component_get_property(FREERTOS_ORIG_INCLUDE_PATH freertos ORIG_INCLUDE_PATH)

idf_component_register(SRC_DIRS integration     # For freertos_test_utils.c
                                integration/event_groups
                                integration/queue
                                integration/stream_buffer
                                integration/tasks
                                integration/timers
                                miscellaneous
                                performance
                                port
                       PRIV_INCLUDE_DIRS . ./integration "${FREERTOS_ORIG_INCLUDE_PATH}"
                       PRIV_REQUIRES cmock test_utils esp_system driver esp_timer)
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
