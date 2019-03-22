set(COMPONENT_SRCDIRS ".")
set(COMPONENT_ADD_INCLUDEDIRS ". ${CMAKE_CURRENT_BINARY_DIR}")

set(COMPONENT_REQUIRES unity test_utils nvs_flash ulp esp_common)

register_component()

add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/test_tjpgd_logo.h"
                COMMAND xxd -i "logo.jpg" "${CMAKE_CURRENT_BINARY_DIR}/test_tjpgd_logo.h"
                WORKING_DIRECTORY ${COMPONENT_PATH}
                DEPENDS "${CMAKE_CURRENT_LIST_DIR}/logo.jpg")

add_custom_target(esp32_test_logo DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/test_tjpgd_logo.h")

add_dependencies(${COMPONENT_TARGET} esp32_test_logo)
