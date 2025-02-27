
if(CONFIG_IDF_TARGET_ESP32)
    set(main_src "secure_boot_main_esp32.c")
else()
    set(main_src "secure_boot_main.c")
endif()

idf_component_register(SRCS "${main_src}" INCLUDE_DIRS ".")
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")

if(CONFIG_EXAMPLE_TARGET_QEMU)
    set(bootloader_unsigned_bin "bootloader-unsigned.bin")
    set(app_unsigned_bin "${PROJECT_BIN}-unsigned.bin")

    add_custom_target(sign_bootloader ALL
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/bootloader/bootloader.bin"
        "${CMAKE_BINARY_DIR}/bootloader/${bootloader_unsigned_bin}"
        COMMAND ${ESPSECUREPY} sign_data --version 2 --keyfile
        ${PROJECT_DIR}/test/secure_boot_signing_key0.pem
        ${PROJECT_DIR}/test/secure_boot_signing_key1.pem
        ${PROJECT_DIR}/test/secure_boot_signing_key2.pem
        -o "${CMAKE_BINARY_DIR}/bootloader/bootloader.bin"
        "${CMAKE_BINARY_DIR}/bootloader/${bootloader_unsigned_bin}"
        COMMAND ${CMAKE_COMMAND} -E echo "Generated signed binary image ${CMAKE_BINARY_DIR}/bootloader/bootloader.bin"
        "from ${CMAKE_BINARY_DIR}/bootloader/${bootloader_unsigned_bin}"
        VERBATIM
        COMMENT "Generated the test-specific signed bootloader")

    add_dependencies(sign_bootloader bootloader)

    add_custom_target(sign_app ALL
        COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_BINARY_DIR}/${PROJECT_BIN}"
        "${CMAKE_BINARY_DIR}/${app_unsigned_bin}"
        COMMAND ${ESPSECUREPY} sign_data --version 2 --keyfile
        ${PROJECT_DIR}/test/secure_boot_signing_key1.pem
        -o "${CMAKE_BINARY_DIR}/${PROJECT_BIN}"
        "${CMAKE_BINARY_DIR}/${app_unsigned_bin}"
        COMMAND ${CMAKE_COMMAND} -E echo "Generated signed binary image ${CMAKE_BINARY_DIR}/${PROJECT_BIN}"
        "from ${CMAKE_BINARY_DIR}/${app_unsigned_bin}"
        VERBATIM
        COMMENT "Generated the test-specific signed application")

    add_dependencies(sign_app app)
endif()
