idf_component_register(SRCS "llvm_memcpy_main.c"
                            "memcpy_baseline.c"
                            "memcpy_optimized.c"
                       INCLUDE_DIRS "")

# Framework markers are ignored on non-Clang (by design). Keep the example
# buildable under the default GCC example CI jobs so P4 board tests can run;
# print a clear warning so local users still know to use Clang for speedup.
if(NOT CMAKE_C_COMPILER_ID MATCHES "Clang")
    message(WARNING
        "examples/system/llvm_memcpy_opt: compiler is not Clang; "
        "ENABLE_LLVM_OPT / idf_component_enable_llvm_opt are ignored. "
        "For the memcpy optimization use:\n"
        "  IDF_TOOLCHAIN=clang idf.py set-target esp32p4\n"
        "  IDF_TOOLCHAIN=clang idf.py build flash monitor")
endif()

idf_component_enable_llvm_opt(SRCS "memcpy_optimized.c")
