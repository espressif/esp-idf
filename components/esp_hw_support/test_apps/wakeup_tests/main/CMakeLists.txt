set(srcs "test_app_main.c"
         "src/io_wakeup_cmd.c")

set(include "."
            "include")

# In order for the cases defined by `TEST_CASE` to be linked into the final elf,
# the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS include
                       REQUIRES unity esp_driver_gpio console test_utils efuse
                       WHOLE_ARCHIVE)
