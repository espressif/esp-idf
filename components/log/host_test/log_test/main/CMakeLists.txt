set(srcs "handler_test.cpp")
# Existing output assertions exercise the text formatter; handler tests also cover binary messages.
if(NOT CONFIG_LOG_MODE_BINARY_EN)
    list(APPEND srcs "log_test.cpp")
endif()

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "."
                    REQUIRES log
                    WHOLE_ARCHIVE)

# Currently 'main' for IDF_TARGET=linux is defined in freertos component.
# Since we are using a freertos mock here, need to let Catch2 provide 'main'.
target_link_libraries(${COMPONENT_LIB} PRIVATE Catch2WithMain)
