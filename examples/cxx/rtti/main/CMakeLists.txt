idf_component_register(SRCS "rtti_example_main.cpp"
                       INCLUDE_DIRS ".")

if(CMAKE_C_COMPILER_ID MATCHES "Clang")
    # Fix Clang 21 potentially-evaluated-expression warning
    # rtti_example_main.cpp: error: expression with side effects will be evaluated
    # despite being used as an operand to 'typeid'
    target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-potentially-evaluated-expression)
endif()
