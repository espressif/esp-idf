idf_component_register(SRCS "host_test_spiffs.c"
                       PRIV_INCLUDE_DIRS "../.." "../../spiffs/src"
                       REQUIRES spiffs unity)

# set BUILD_DIR because test uses a file created in the build directory
target_compile_definitions(${COMPONENT_LIB} PRIVATE "BUILD_DIR=\"${build_dir}\"")
