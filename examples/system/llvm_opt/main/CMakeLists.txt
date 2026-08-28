idf_component_register(SRCS "llvm_opt_main.c"
                       INCLUDE_DIRS ""
                       ENABLE_LLVM_OPT)

if(NOT CMAKE_C_COMPILER_ID MATCHES "Clang")
    message(WARNING
        "examples/system/llvm_opt: compiler is not Clang; "
        "ENABLE_LLVM_OPT is ignored. For real LLVM opt flags use:\n"
        "  IDF_TOOLCHAIN=clang idf.py set-target esp32p4\n"
        "  IDF_TOOLCHAIN=clang idf.py build flash monitor")
endif()
