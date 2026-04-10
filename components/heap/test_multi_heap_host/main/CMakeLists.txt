idf_component_register(SRCS "test_multi_heap.cpp"
                            "../../multi_heap_poisoning.c"
                            "../../multi_heap.c"
                            "../../tlsf/tlsf.c"
                       INCLUDE_DIRS
                            "../../include"
                            "../../tlsf"
                            "../../tlsf/include"
                       WHOLE_ARCHIVE)

# Currently 'main' for IDF_TARGET=linux is defined in freertos component.
# Since we are using a freertos mock here, need to let Catch2 provide 'main'.
target_link_libraries(${COMPONENT_LIB} PRIVATE Catch2WithMain)
