idf_build_get_property(target IDF_TARGET)
if(NOT "${target}" STREQUAL "esp32s3")
    return()
endif()
