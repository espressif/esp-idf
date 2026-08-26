# Function placement in IRAM section

The heap component is compiled and linked in a way that minimizes the utilization of the IRAM section of memory without impacting the performance of its core functionalities. For this reason, the heap component API provided through [esp_heap_caps.h](./include/esp_heap_caps.h) and [esp_heap_caps_init.h](./include/esp_heap_caps_init.h) can be sorted into two sets of functions.

1. The performance related functions placed into the IRAM by using the `IRAM_ATTR` defined in [esp_attr.h](./../../components/esp_common/include/esp_attr.h) (e.g., `heap_caps_malloc`, `heap_caps_free`, `heap_caps_realloc`, etc.)

2. The functions that does not require the best of performance placed in the flash (e.g., `heap_caps_print_heap_info`, `heap_caps_dump`, `heap_caps_dump_all`, etc.)

With that in  mind, all the functions defined in [multi_heap.c](./multi_heap.c), [multi_heap_poisoning.c](./multi_heap_poisoning.c) and [tlsf.c](./tlsf/tlsf.c) that are directly or indirectly called from one of the heap component API functions placed in IRAM have to also be placed in IRAM. Symmetrically, the functions directly or indirectly called from one of the heap component API functions placed in flash will also be placed in flash.