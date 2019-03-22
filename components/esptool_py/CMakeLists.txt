register_config_only_component()

if(IDF_BUILD_ARTIFACTS)
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
configure_file(${COMPONENT_PATH}/flash_project_args.in
               ${CMAKE_CURRENT_BINARY_DIR}/flash_project_args.in)

file(GENERATE OUTPUT ${CMAKE_BINARY_DIR}/flash_project_args
              INPUT ${CMAKE_CURRENT_BINARY_DIR}/flash_project_args.in)

configure_file(${COMPONENT_PATH}/flasher_args.json.in
               ${CMAKE_CURRENT_BINARY_DIR}/flasher_args.json.in)

file(GENERATE OUTPUT ${CMAKE_BINARY_DIR}/flasher_args.json
              INPUT ${CMAKE_CURRENT_BINARY_DIR}/flasher_args.json.in)
endif()