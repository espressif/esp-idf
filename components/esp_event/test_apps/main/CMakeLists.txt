set(srcs "test_event_main.c"
         "test_default_loop.c"
         "test_event.c")

idf_component_register(SRCS ${srcs}
                       PRIV_INCLUDE_DIRS . ../../private_include
                       PRIV_REQUIRES esp_event driver esp_timer unity test_utils
                       WHOLE_ARCHIVE)
