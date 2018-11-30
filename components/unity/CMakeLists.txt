set(COMPONENT_SRCS "unity/src/unity.c"
                   "unity_port_esp32.c")

set(COMPONENT_ADD_INCLUDEDIRS "include"
                              "unity/src")

if(CONFIG_UNITY_ENABLE_IDF_TEST_RUNNER)
    list(APPEND COMPONENT_SRCS "unity_runner.c")
endif()

if(CONFIG_UNITY_ENABLE_FIXTURE)
    list(APPEND COMPONENT_SRCS "unity/extras/fixture/src/unity_fixture.c")
endif()

register_component()

target_compile_definitions(${COMPONENT_TARGET} PUBLIC
    -DUNITY_INCLUDE_CONFIG_H
)

if(GCC_NOT_5_2_0)
    component_compile_options(-Wno-unused-const-variable)
endif()
