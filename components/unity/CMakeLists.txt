idf_build_get_property(target IDF_TARGET)

set(srcs
    "unity/src/unity.c"
    "unity_compat.c"
)

set(includes
    "include"
    "unity/src")

set(requires "")

if(CONFIG_UNITY_ENABLE_BACKTRACE_ON_FAIL)
    list(APPEND COMPONENT_PRIV_INCLUDEDIRS "include/priv")
endif()

if(CONFIG_UNITY_ENABLE_IDF_TEST_RUNNER)
    list(APPEND srcs "unity_runner.c")
    if(NOT "${target}" STREQUAL "linux")
        list(APPEND srcs "unity_utils_freertos.c" "unity_utils_cache.c")
    endif()
    list(APPEND requires "freertos")
endif()

if(CONFIG_UNITY_ENABLE_FIXTURE)
    list(APPEND srcs "unity/extras/fixture/src/unity_fixture.c")
    list(APPEND includes "unity/extras/fixture/src")
endif()

list(APPEND srcs "unity_utils_memory.c")

if(NOT "${target}" STREQUAL "linux")
    list(APPEND srcs "unity_port_esp32.c")
    list(APPEND srcs "port/esp/unity_utils_memory_esp.c")
else()
    list(APPEND srcs "unity_port_linux.c")
    list(APPEND srcs "port/linux/unity_utils_memory_linux.c")
endif()

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS ${includes}
                    REQUIRES ${requires})

if(CONFIG_UNITY_ENABLE_IDF_TEST_RUNNER)
    idf_component_optional_requires(PRIVATE spi_flash)
endif()

target_compile_definitions(${COMPONENT_LIB} PUBLIC
    -DUNITY_INCLUDE_CONFIG_H
)

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-unused-const-variable)
