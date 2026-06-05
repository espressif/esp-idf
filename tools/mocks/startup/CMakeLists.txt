# This is a manual mock that supplies `main()` if FreeRTOS is mocked
idf_component_register(SRCS "startup_mock.c"
                       REQUIRES main esp_event)

# Prevent esp_system from providing main() since this component provides it
idf_component_get_property(esp_system_lib esp_system COMPONENT_LIB)
target_compile_definitions(${esp_system_lib} PRIVATE ESP_SYSTEM_LINUX_NO_MAIN)
