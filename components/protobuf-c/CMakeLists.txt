idf_component_register(SRCS "protobuf-c/protobuf-c/protobuf-c.c"
                    INCLUDE_DIRS protobuf-c)

if(CONFIG_COMPILER_OPTIMIZATION_ASSERTIONS_DISABLE)
   # some variables are only used by asserts
   target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-unused-but-set-variable)
endif()
