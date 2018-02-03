#
# Component Makefile
#

COMPONENT_OBJS := heap_caps_init.o heap_caps.o multi_heap.o heap_trace.o

ifndef CONFIG_HEAP_POISONING_DISABLED
COMPONENT_OBJS += multi_heap_poisoning.o
endif

ifdef CONFIG_HEAP_TRACING

WRAP_FUNCTIONS = calloc malloc free realloc heap_caps_malloc heap_caps_free heap_caps_realloc
WRAP_ARGUMENT := -Wl,--wrap=

COMPONENT_ADD_LDFLAGS = -l$(COMPONENT_NAME) $(addprefix $(WRAP_ARGUMENT),$(WRAP_FUNCTIONS))

endif
