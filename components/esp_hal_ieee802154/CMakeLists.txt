idf_build_get_property(target IDF_TARGET)

set(includes "include")
set(srcs "")

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${target}/include")
    list(APPEND includes "${target}/include")
endif()

if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${target}/ieee802154_periph.c")
    list(APPEND srcs "${target}/ieee802154_periph.c")
endif()

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${includes})
