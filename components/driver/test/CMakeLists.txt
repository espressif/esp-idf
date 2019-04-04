set(COMPONENT_SRCDIRS ". param_test")
set(COMPONENT_ADD_INCLUDEDIRS "include param_test/include")

set(COMPONENT_REQUIRES unity test_utils driver nvs_flash)

register_component()