
set(COMPONENT_SRCS "light_driver.c"
                    "iot_led.c")

set(COMPONENT_ADD_INCLUDEDIRS ". include")

# requirements can't depend on config
set(COMPONENT_REQUIRES example_nvs driver)

register_component()
