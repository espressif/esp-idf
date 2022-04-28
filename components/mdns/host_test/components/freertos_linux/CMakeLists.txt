idf_component_register(SRCS freertos_linux.c queue_unique_ptr.cpp
                       INCLUDE_DIRS include
                       REQUIRES esp_system_protocols_linux)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
target_link_libraries(${COMPONENT_LIB} PRIVATE Threads::Threads)

set_target_properties(${COMPONENT_LIB} PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS ON
)
