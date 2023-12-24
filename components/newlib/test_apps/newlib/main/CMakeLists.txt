idf_component_register(SRCS
                       "test_app_main.c"
                       "test_atomic.c"
                       "test_file.c"
                       "test_locks.c"
                       "test_misc.c"
                       "test_newlib.c"
                       "test_setjmp.c"
                       "test_stdatomic.c"
                       "test_time.c"
                       PRIV_REQUIRES unity vfs cmock driver esp_timer spi_flash test_utils pthread esp_psram
                       WHOLE_ARCHIVE)
