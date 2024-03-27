# NOTE: This kind of mocking currently works on Linux targets only.
#       On Espressif chips, too many dependencies are missing at the moment.
message(STATUS "building FREERTOS MOCKS (only task, event-groups and queue)")

idf_component_get_property(original_freertos_dir freertos COMPONENT_OVERRIDEN_DIR)

set(kernel_dir "${original_freertos_dir}/FreeRTOS-Kernel")

set(include_dirs
    "${kernel_dir}/include"
    "${original_freertos_dir}/config/include" # For "freertos/FreeRTOSConfig.h"
    "${original_freertos_dir}/config/include/freertos" # For "FreeRTOSConfig.h"
    "${original_freertos_dir}/config/linux/include" # For "freertos/FreeRTOSConfig_arch.h"
    "${original_freertos_dir}/esp_additions/include"
    # Required because CMock tries to include "idf_additions.h" instead of "freertos/idf_additions.h"
    "${original_freertos_dir}/esp_additions/include/freertos"
    "${kernel_dir}/portable/linux/include" # For "spinlock.h"
    "${kernel_dir}/portable/linux/include/freertos" # For "portmacro.h"
    "${kernel_dir}/include/freertos" # this is due to the way includes are generated in CMock (without freertos prefix)
)

idf_component_mock(INCLUDE_DIRS ${include_dirs}
    REQUIRES esp_common
    MOCK_HEADER_FILES
    ${original_freertos_dir}/esp_additions/include/freertos/idf_additions.h
    ${original_freertos_dir}/FreeRTOS-Kernel/include/freertos/task.h
    ${original_freertos_dir}/FreeRTOS-Kernel/include/freertos/event_groups.h
    ${original_freertos_dir}/FreeRTOS-Kernel/include/freertos/queue.h)

idf_component_get_property(freertos_lib freertos COMPONENT_LIB)
target_compile_definitions(${freertos_lib} PUBLIC "projCOVERAGE_TEST=0")
