# Mark __cxa_guard_dummy as undefined so that implementation of static guards 
# is taken from cxx_guards.o instead of libstdc++.a
COMPONENT_ADD_LDFLAGS += -u __cxa_guard_dummy
