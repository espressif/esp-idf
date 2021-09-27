idf_component_register(SRCS esp_timer_linux.c timer_task.cpp
                       INCLUDE_DIRS include
                       REQUIRES esp_system_protocols_linux freertos_linux)

target_compile_features(${COMPONENT_LIB} PRIVATE cxx_std_17)
