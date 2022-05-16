idf_component_register(SRCS "tinycbor/src/cborencoder_close_container_checked.c"
                            "tinycbor/src/cborencoder.c"
                            "tinycbor/src/cborerrorstrings.c"
                            "tinycbor/src/cborparser_dup_string.c"
                            "tinycbor/src/cborparser.c"
                            "tinycbor/src/cborpretty_stdio.c"
                            "tinycbor/src/cborpretty.c"
                            "tinycbor/src/cbortojson.c"
                            "tinycbor/src/cborvalidation.c"
                            "tinycbor/src/open_memstream.c"
                    INCLUDE_DIRS "port/include"
                    PRIV_INCLUDE_DIRS "tinycbor/src")

# for open_memstream.c
set_source_files_properties(tinycbor/src/open_memstream.c PROPERTIES COMPILE_DEFINITIONS "__linux__")

# workaround for the fact that we are passing -ffreestanding to Clang
if(CMAKE_C_COMPILER_ID MATCHES "Clang")
    target_compile_options(${COMPONENT_LIB} PRIVATE "-U __STDC_HOSTED__")
endif()
