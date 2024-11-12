cmake_minimum_required(VERSION 3.16)

idf_build_get_property(esp_tee_build ESP_TEE_BUILD)

if(NOT esp_tee_build)
    return()
endif()

idf_component_register(SRCS "example_service.c"
                       INCLUDE_DIRS include
                       PRIV_REQUIRES main)
