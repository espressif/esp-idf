
set(COMPONENT_SRCS "genie_event.c"
                   "genie_mesh.c"
                   "genie_reset.c"
                   "genie_util.c"
                   "genie_model_srv.c"
                   "genie_timer.c")

set(COMPONENT_ADD_INCLUDEDIRS ". include")

set(COMPONENT_REQUIRES bt nvs_flash example_nvs)

register_component()
