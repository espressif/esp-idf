idf_component_register(SRC_DIRS .
                       PRIV_INCLUDE_DIRS . ${CMAKE_CURRENT_BINARY_DIR}
                       PRIV_REQUIRES cmock test_utils)

add_custom_command(OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/test_tjpgd_logo.h"
                COMMAND xxd -i "logo.jpg" "${CMAKE_CURRENT_BINARY_DIR}/test_tjpgd_logo.h"
                WORKING_DIRECTORY ${COMPONENT_DIR}
                DEPENDS "${CMAKE_CURRENT_LIST_DIR}/logo.jpg")
add_custom_target(test_logo DEPENDS "${CMAKE_CURRENT_BINARY_DIR}/test_tjpgd_logo.h")
add_dependencies(${COMPONENT_LIB} test_logo)
