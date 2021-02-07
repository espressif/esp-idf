#include <cstdlib>
#include <cstdio>
#include <exception>
#include <bits/functexcept.h>
#include <sdkconfig.h>

#ifndef CONFIG_COMPILER_CXX_EXCEPTIONS

extern "C" void abort_expect_void(const void *context)
{
    abort();
}

extern "C" void *abort_expect_void_and_return(const void *context)
{
    abort();
    return nullptr;
}

extern "C" void *forward_abort_uw_ctx(struct _Unwind_Context *context)
{
    return abort_expect_void_and_return((void*) context);
}

template<typename T>
static T abort_return()
{
    abort();
    return static_cast<T>(0);
}

// unwind-dw2-fde.o
extern "C" void __wrap__Unwind_SetEnableExceptionFdeSorting(unsigned char enable)
{
    abort();
}

extern "C" void __wrap___register_frame_info_bases (const void *begin, struct object *ob, void *tbase, void *dbase)
{
    abort();
}

extern "C" void __wrap___register_frame_info (const void *begin, struct object *ob)
{
    abort();
}

extern "C" void __wrap___register_frame_info_table_bases (void *begin, struct object *ob, void *tbase, void *dbase)
{
    abort();
}

extern "C" void __wrap___register_frame_info_table (void *begin, struct object *ob)
{
    abort();
}


extern "C" void __wrap___register_frame (void *begin)
        __attribute__((alias("abort_expect_void")));

extern "C" void __wrap___register_frame_table (void *begin)
        __attribute__((alias("abort_expect_void")));

extern "C" void *__wrap___deregister_frame_info_bases (const void *begin)
        __attribute__((alias("abort_expect_void_and_return")));

extern "C" void *__wrap___deregister_frame_info (const void *begin)
        __attribute__((alias("abort_expect_void_and_return")));

extern "C" void __wrap___deregister_frame (void *begin)
        __attribute__((alias("abort_expect_void")));

typedef void* fde;

extern "C" const fde * __wrap__Unwind_Find_FDE (void *pc, struct dwarf_eh_bases *bases)
{
    return abort_return<fde*>();
}

// unwind-dw2.o (riscv), unwind-dw2-xtensa.o (xtensa)
typedef void* _Unwind_Ptr;
typedef int _Unwind_Word;

extern "C" _Unwind_Word __wrap__Unwind_GetGR (struct _Unwind_Context *context, int index)
{
    return abort_return<_Unwind_Word>();
}

extern "C" _Unwind_Word __wrap__Unwind_GetCFA (struct _Unwind_Context *context)
{
    return abort_return<_Unwind_Word>();
}

extern "C" void __wrap__Unwind_SetIP (struct _Unwind_Context *context, _Unwind_Ptr val)
{
    abort();
}

extern "C" void __wrap__Unwind_SetGR (struct _Unwind_Context *context, int index, _Unwind_Word val)
{
    abort();
}

extern "C" _Unwind_Ptr __wrap__Unwind_GetIPInfo (struct _Unwind_Context *context, int *ip_before_insn)
{
    return abort_return<_Unwind_Ptr>();
}

extern "C" _Unwind_Ptr __wrap__Unwind_GetIP (struct _Unwind_Context *context)
        __attribute__((alias("forward_abort_uw_ctx")));

extern "C" _Unwind_Ptr __wrap__Unwind_GetRegionStart (struct _Unwind_Context *context)
        __attribute__((alias("forward_abort_uw_ctx")));

extern "C" _Unwind_Ptr __wrap__Unwind_GetDataRelBase (struct _Unwind_Context *context)
        __attribute__((alias("forward_abort_uw_ctx")));

extern "C" _Unwind_Ptr __wrap__Unwind_GetTextRelBase (struct _Unwind_Context *context)
        __attribute__((alias("forward_abort_uw_ctx")));

extern "C" void *__wrap__Unwind_GetLanguageSpecificData (struct _Unwind_Context *context)
        __attribute__((alias("forward_abort_uw_ctx")));

extern "C" void *__wrap__Unwind_FindEnclosingFunction (void *pc)
        __attribute__((alias("abort_expect_void_and_return")));

struct frame_state *__frame_state_for (void *pc_target, struct frame_state *state_in)
{
    return abort_return<struct frame_state *>();
}

// unwind.inc
typedef int _Unwind_Reason_Code;
typedef int _Unwind_Action;
typedef int _Unwind_Exception_Class;
typedef int* _Unwind_Trace_Fn;
typedef int* _Unwind_Stop_Fn;

extern "C" void __wrap__Unwind_Resume (struct _Unwind_Exception *exc)
{
    abort();
}

extern "C" void __wrap__Unwind_DeleteException (struct _Unwind_Exception *exc)
{
    abort();
}


extern "C" _Unwind_Reason_Code __wrap__Unwind_RaiseException(struct _Unwind_Exception *exc)
{
    return abort_return<_Unwind_Reason_Code>();
}

extern "C" _Unwind_Reason_Code __wrap__Unwind_Resume_or_Rethrow (struct _Unwind_Exception *exc)
        __attribute__((alias("__wrap__Unwind_RaiseException")));

extern "C" _Unwind_Reason_Code __wrap__Unwind_ForcedUnwind (struct _Unwind_Exception *exc,
        _Unwind_Stop_Fn stop, void * stop_argument)
{
    return abort_return<_Unwind_Reason_Code>();
}

extern "C" _Unwind_Reason_Code __wrap__Unwind_Backtrace(_Unwind_Trace_Fn trace, void * trace_argument)
{
    return abort_return<_Unwind_Reason_Code>();
}

// eh_personality.o
extern "C" void __wrap___cxa_call_unexpected (void *exc_obj_in)
        __attribute__((alias("abort_expect_void")));

extern "C" _Unwind_Reason_Code __wrap___gxx_personality_v0 (int version,
        _Unwind_Action actions,
        _Unwind_Exception_Class exception_class,
        struct _Unwind_Exception *ue_header,
        struct _Unwind_Context *context)
{
    return abort_return<_Unwind_Reason_Code>();
}

#endif // CONFIG_COMPILER_CXX_EXCEPTIONS
