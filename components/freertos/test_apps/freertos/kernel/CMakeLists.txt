# Register all of the "kernel" tests as a component

# For refactored FreeRTOS unit tests, we need to support #include "xxx.h" of FreeRTOS headers
idf_component_get_property(FREERTOS_ORIG_INCLUDE_PATH freertos ORIG_INCLUDE_PATH)

set(src_dirs
    "."                 # For freertos_test_utils.c
    "event_groups"
    "queue"
    "stream_buffer"
    "tasks"
    "timers")

set(priv_include_dirs
    "."                                 # For portTestMacro.h
    "${FREERTOS_ORIG_INCLUDE_PATH}")    # FreeRTOS headers via`#include "xxx.h"`

# In order for the cases defined by `TEST_CASE` in "kernel" to be linked into
# the final elf, the component can be registered as WHOLE_ARCHIVE
idf_component_register(SRC_DIRS ${src_dirs}
                       PRIV_INCLUDE_DIRS ${priv_include_dirs}
                       PRIV_REQUIRES test_utils driver
                       WHOLE_ARCHIVE)
