# Register all of the "kernel" tests as a component

# For refactored FreeRTOS unit tests, we need to support #include "xxx.h" of FreeRTOS headers
idf_component_get_property(FREERTOS_ORIG_INCLUDE_PATH freertos ORIG_INCLUDE_PATH)

set(src_dirs
    "."                 # For freertos_test_utils.c
    "tasks"
    "queue"
    "port"
    "stream_buffer"
    "timers")

set(priv_include_dirs
    "."                                 # For portTestMacro.h
    "${FREERTOS_ORIG_INCLUDE_PATH}")    # FreeRTOS headers via`#include "xxx.h"`

idf_component_register(SRC_DIRS ${src_dirs}
                       PRIV_INCLUDE_DIRS ${priv_include_dirs}
                       PRIV_REQUIRES unity
                       WHOLE_ARCHIVE)

target_compile_options(${COMPONENT_LIB} PRIVATE
  -Wno-pointer-to-int-cast
)
