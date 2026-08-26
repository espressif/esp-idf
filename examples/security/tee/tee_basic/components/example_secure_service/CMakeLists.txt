cmake_minimum_required(VERSION 3.22)

idf_build_get_property(esp_tee_build ESP_TEE_BUILD)

set(srcs)
set(include_dirs include)

if(esp_tee_build)
    list(APPEND srcs "example_service.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${include_dirs}
                       PRIV_REQUIRES main)
