set(COMPONENT_SRCS "cJSON/cJSON.c"
                   "cJSON/cJSON_Utils.c"
                   "cJSON/test.c")
set(COMPONENT_ADD_INCLUDEDIRS cJSON)

set(COMPONENT_REQUIRES "")

register_component()
