# Mark __cxa_guard_dummy as undefined so that implementation of static guards 
# is taken from cxx_guards.o instead of libstdc++.a
COMPONENT_ADD_LDFLAGS += -u __cxa_guard_dummy

ifndef CONFIG_COMPILER_CXX_EXCEPTIONS
# If exceptions are disabled, ensure our fatal exception
# hooks are preferentially linked over libstdc++ which
# has full exception support
WRAP_FUNCTIONS = _Unwind_SetEnableExceptionFdeSorting __register_frame_info_bases __register_frame_info __register_frame __register_frame_info_table_bases __register_frame_info_table __register_frame_table __deregister_frame_info_bases __deregister_frame_info _Unwind_Find_FDE _Unwind_GetGR _Unwind_GetCFA _Unwind_GetIP _Unwind_GetIPInfo _Unwind_GetRegionStart _Unwind_GetDataRelBase _Unwind_GetTextRelBase _Unwind_SetIP _Unwind_SetGR _Unwind_GetLanguageSpecificData _Unwind_FindEnclosingFunction _Unwind_Resume _Unwind_RaiseException _Unwind_DeleteException _Unwind_ForcedUnwind _Unwind_Resume_or_Rethrow _Unwind_Backtrace __cxa_call_unexpected __gxx_personality_v0
WRAP_ARGUMENT := -Wl,--wrap=

COMPONENT_ADD_LDFLAGS = -l$(COMPONENT_NAME) $(addprefix $(WRAP_ARGUMENT),$(WRAP_FUNCTIONS))
COMPONENT_ADD_LDFLAGS += -u __cxx_fatal_exception

endif # CONFIG_COMPILER_CXX_EXCEPTIONS

COMPONENT_ADD_INCLUDEDIRS =
