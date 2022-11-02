idf_component_get_property(original_heap_dir heap COMPONENT_OVERRIDEN_DIR)

idf_component_register(SRCS heap_mock.c
  INCLUDE_DIRS "${original_heap_dir}/include")
