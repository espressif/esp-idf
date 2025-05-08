cmake_minimum_required(VERSION 3.16)

idf_build_get_property(esp_tee_build ESP_TEE_BUILD)

set(srcs)
set(priv_requires)

if(esp_tee_build)
    list(APPEND srcs "src/test_dummy_srv.c"
                     "src/test_interrupt.c"
                     "src/test_panic.c"
                     "src/test_sec_srv.c"
                     "src/test_heap.c")
    list(APPEND priv_requires main)
else()
    list(APPEND srcs "src/test_dummy_srv_wrapper.c")
    list(APPEND priv_requires esp_tee)
endif()


idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS include
                       PRIV_REQUIRES ${priv_requires})
