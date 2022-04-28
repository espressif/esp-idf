# NOTE: This kind of mocking currently works on Linux targets only.
#       On Espressif chips, too many dependencies are missing at the moment.
message(STATUS "building FREERTOS MOCKS (only task and queue)")

idf_component_get_property(original_freertos_dir freertos COMPONENT_OVERRIDEN_DIR)

set(include_dirs
    "${original_freertos_dir}/FreeRTOS-Kernel/include"
    "${original_freertos_dir}/esp_additions/include"
    "${original_freertos_dir}/esp_additions/include/freertos"
    "${original_freertos_dir}/FreeRTOS-Kernel/include/freertos" # this is due to the way includes are generated in CMock
    "${original_freertos_dir}/FreeRTOS-Kernel/portable/linux/include")

idf_component_mock(INCLUDE_DIRS ${include_dirs}
    REQUIRES esp_common
    MOCK_HEADER_FILES
    ${original_freertos_dir}/FreeRTOS-Kernel/include/freertos/task.h
    ${original_freertos_dir}/FreeRTOS-Kernel/include/freertos/queue.h)
