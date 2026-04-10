idf_component_register(SRCS "test_main.c"
                       INCLUDE_DIRS ".")

# Update include path to not include the directory, i.e. "task.h" instead of "freertos/task.h"
idf_component_get_property(FREERTOS_ORIG_INCLUDE_PATH freertos ORIG_INCLUDE_PATH)
target_include_directories(${COMPONENT_TARGET} PRIVATE "${FREERTOS_ORIG_INCLUDE_PATH}")
