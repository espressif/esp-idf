# This is a manual mock that supplies `main()` if FreeRTOS is mocked
idf_component_register(SRCS "startup_mock.c"
                       REQUIRES main esp_event)
