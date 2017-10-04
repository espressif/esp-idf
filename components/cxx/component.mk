# Mark __cxa_guard_dummy as undefined so that implementation of static guards 
# is taken from cxx_guards.o instead of libstdc++.a
COMPONENT_ADD_LDFLAGS += -u __cxa_guard_dummy

ifndef CONFIG_CXX_EXCEPTIONS
# If exceptions are disabled, ensure our fatal exception
# hooks are preferentially linked over libstdc++ which
# has full exception support
COMPONENT_ADD_LDFLAGS += -u __cxx_fatal_exception
endif

