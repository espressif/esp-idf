set(COMPONENT_PRIV_REQUIRES bootloader)
register_component()

string(REPLACE ";" " " ESPTOOLPY_FLASH_PROJECT_OPTIONS "${ESPTOOLPY_ELF2IMAGE_FLASH_OPTIONS}")
set(ESPTOOLPY_FLASH_PROJECT_OPTIONS
    "${ESPTOOLPY_FLASH_PROJECT_OPTIONS}"
    )

if(CONFIG_SECURE_BOOT_ENABLED)
    set(ESPTOOLPY_FLASH_PROJECT_OPTIONS "")
endif()

# Generate the flash project args and the flasher args json file using the accumulated values
# from esptool_py_flash_project_args calls. The file is first configured using configure_file() for all variable values,
# and then generated using file(GENERATE... for generator expressions.
configure_file(${COMPONENT_DIR}/flash_project_args.in
               ${CMAKE_CURRENT_BINARY_DIR}/flash_project_args.in)

file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/flash_project_args.in2
              INPUT ${CMAKE_CURRENT_BINARY_DIR}/flash_project_args.in)
file(GENERATE OUTPUT ${CMAKE_BINARY_DIR}/flash_project_args
              INPUT ${CMAKE_CURRENT_BINARY_DIR}/flash_project_args.in2)

configure_file(${COMPONENT_DIR}/flasher_args.json.in
               ${CMAKE_CURRENT_BINARY_DIR}/flasher_args.json.in)

file(GENERATE OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/flasher_args.json.in2
              INPUT ${CMAKE_CURRENT_BINARY_DIR}/flasher_args.json.in)
file(GENERATE OUTPUT ${CMAKE_BINARY_DIR}/flasher_args.json
              INPUT ${CMAKE_CURRENT_BINARY_DIR}/flasher_args.json.in2)

set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES
    "${CMAKE_CURRENT_BINARY_DIR}/flash_project_args.in"
    "${CMAKE_CURRENT_BINARY_DIR}/flash_project_args.in2"
    "${CMAKE_BINARY_DIR}/flash_project_args"
    "${CMAKE_CURRENT_BINARY_DIR}/flasher_args.json.in"
    "${CMAKE_CURRENT_BINARY_DIR}/flasher_args.json.in2"
    "${CMAKE_BINARY_DIR}/flasher_args.json")
