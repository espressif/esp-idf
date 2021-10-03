idf_build_get_property(target IDF_TARGET)

set(srcs
    "unity/src/unity.c")

set(includes
    "include"
    "unity/src")

set(requires "")

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

if(NOT "${target}" STREQUAL "linux")
    list(APPEND srcs "unity_port_esp32.c")
endif()

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS ${includes}
                    REQUIRES ${requires})

if(NOT "${target}" STREQUAL "linux")
    target_compile_definitions(${COMPONENT_LIB} PUBLIC
        -DUNITY_INCLUDE_CONFIG_H
    )
endif()

target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-unused-const-variable)
