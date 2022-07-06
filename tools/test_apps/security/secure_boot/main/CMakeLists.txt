
if(CONFIG_IDF_TARGET_ESP32)
    set(main_src "secure_boot_main_esp32.c")
else()
    set(main_src "secure_boot_main.c")
endif()

idf_component_register(SRCS "${main_src}" INCLUDE_DIRS ".")
target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
