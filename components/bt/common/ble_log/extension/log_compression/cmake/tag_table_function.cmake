# Usage：
#   set(MY_TABLE  CONFIG_FOO_ON  CONFIG_FOO_KEEP  TAG_FOO ...)
#   tag_table_to_lists(MY_TABLE  OUT_TAGS  OUT_PRESERVE)
function(tag_table_to_lists TABLE_NAME OUT_TAGS OUT_PRESERVE)
    set(_tags "")
    set(_preserve "")
    set(_map ${${TABLE_NAME}})

    list(LENGTH _map _len)
    math(EXPR _stop "${_len} - 1")

    foreach(i RANGE 0 ${_stop} 3)
        math(EXPR _i1 "${i} + 1")
        math(EXPR _i2 "${i} + 2")

        list(GET _map ${i}   _comp)
        list(GET _map ${_i1} _pres)
        list(GET _map ${_i2} _tag)

        if(${_comp})
            list(APPEND _tags "${_tag}")
            if(${_pres})
                list(APPEND _preserve "${_tag}")
            endif()
        endif()
    endforeach()

    list(JOIN _tags ", " _tags_str)
    list(JOIN _preserve ", " _preserve_str)

    set(${OUT_TAGS} "${_tags_str}"       PARENT_SCOPE)
    set(${OUT_PRESERVE} "${_preserve_str}" PARENT_SCOPE)
endfunction()
