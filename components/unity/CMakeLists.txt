set(srcs
    "unity/src/unity.c")

set(includes
    "include"
    "unity/src")

if(CONFIG_UNITY_ENABLE_BACKTRACE_ON_FAIL)
    list(APPEND COMPONENT_PRIV_INCLUDEDIRS "include/priv")
endif()

if(CONFIG_UNITY_ENABLE_IDF_TEST_RUNNER)
    list(APPEND srcs "unity_runner.c")
endif()

if(CONFIG_UNITY_ENABLE_FIXTURE)
    list(APPEND srcs "unity/extras/fixture/src/unity_fixture.c")
    list(APPEND includes "unity/extras/fixture/src")
endif()

if(${IDF_TARGET} STREQUAL "linux")
    message(STATUS "adding linux stuff...")
    idf_component_get_property(spi_flash_dir spi_flash COMPONENT_DIR)
    list(APPEND includes "${spi_flash_dir}/sim/stubs/esp_common")
else()
    list(APPEND srcs "unity_port_esp32.c")
endif()

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS ${includes})

if(NOT ${IDF_TARGET} STREQUAL "linux")
    target_compile_definitions(${COMPONENT_LIB} PUBLIC
        -DUNITY_INCLUDE_CONFIG_H
    )
endif()

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-unused-const-variable)
