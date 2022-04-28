idf_component_register(SRC_DIRS integration/event_groups
                                integration/queue
                                integration/stream_buffer
                                integration/tasks
                                integration/timers
                                miscellaneous
                                performance
                                port
                       PRIV_INCLUDE_DIRS .
                       PRIV_REQUIRES cmock test_utils esp_system driver)
