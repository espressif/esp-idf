set(srcs 
    "unity/src/unity.c"
    "unity_port_esp32.c")

if(CONFIG_UNITY_ENABLE_BACKTRACE_ON_FAIL)
    list(APPEND COMPONENT_PRIV_INCLUDEDIRS "include/priv")
endif()

if(CONFIG_UNITY_ENABLE_IDF_TEST_RUNNER)
    list(APPEND srcs "unity_runner.c")
endif()

if(CONFIG_UNITY_ENABLE_FIXTURE)
    list(APPEND srcs "unity/extras/fixture/src/unity_fixture.c")
endif()

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "include" "unity/src")

target_compile_definitions(${COMPONENT_LIB} PUBLIC
    -DUNITY_INCLUDE_CONFIG_H
)

if(GCC_NOT_5_2_0)
    target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-unused-const-variable)
endif()
