#include <cstdlib>
#include <cstdio>
#include <exception>
#include <bits/functexcept.h>
#include <sdkconfig.h>

#ifndef CONFIG_CXX_EXCEPTIONS

const char *FATAL_EXCEPTION = "Fatal C++ exception: ";

extern "C" void __cxx_fatal_exception(void)
{
    abort();
}

extern "C" void __cxx_fatal_exception_message(const char *msg)
{
    printf("%s%s\n", FATAL_EXCEPTION, msg);
    abort();
}

extern "C" void __cxx_fatal_exception_int(int i)
{
    printf("%s (%d)\n", FATAL_EXCEPTION, i);
    abort();
}

void std::__throw_bad_exception(void) __attribute__((alias("__cxx_fatal_exception")));

void std::__throw_bad_alloc(void) __attribute__((alias("__cxx_fatal_exception")));

void std::__throw_bad_cast(void) __attribute__((alias("__cxx_fatal_exception")));

void std::__throw_bad_typeid(void) __attribute__((alias("__cxx_fatal_exception")));

void std::__throw_logic_error(const char*) __attribute__((alias("__cxx_fatal_exception_message")));

void std::__throw_domain_error(const char*) __attribute__((alias("__cxx_fatal_exception_message")));

void std::__throw_invalid_argument(const char*) __attribute__((alias("__cxx_fatal_exception_message")));

void std::__throw_length_error(const char*) __attribute__((alias("__cxx_fatal_exception_message")));

void std::__throw_out_of_range(const char*) __attribute__((alias("__cxx_fatal_exception_message")));

void std::__throw_out_of_range_fmt(const char*, ...) __attribute__((alias("__cxx_fatal_exception_message")));

void std::__throw_runtime_error(const char*) __attribute__((alias("__cxx_fatal_exception_message")));

void std::__throw_range_error(const char*) __attribute__((alias("__cxx_fatal_exception_message")));

void std::__throw_overflow_error(const char*) __attribute__((alias("__cxx_fatal_exception_message")));

void std::__throw_underflow_error(const char*) __attribute__((alias("__cxx_fatal_exception_message")));

void std::__throw_ios_failure(const char*) __attribute__((alias("__cxx_fatal_exception_message")));

void std::__throw_system_error(int) __attribute__((alias("__cxx_fatal_exception_int")));

void std::__throw_bad_function_call(void) __attribute__((alias("__cxx_fatal_exception")));

void std::__throw_future_error(int) __attribute__((alias("__cxx_fatal_exception_int")));


/* The following definitions are needed because libstdc++ is also compiled with
   __throw_exception_again defined to throw, and some other exception code in a few places.

   This cause exception handler code to be emitted in the library even though it's mostly
   unreachable (as any libstdc++ "throw" will first call one of the above stubs, which will abort).

   If these are left out, a bunch of unwanted exception handler code is linked.

   Note: these function prototypes are not correct.
*/

extern "C" void __cxa_allocate_exception(void) __attribute__((alias("__cxx_fatal_exception")));
extern "C" void __cxa_allocate_dependent_exception(void) __attribute__((alias("__cxx_fatal_exception")));
extern "C" void __cxa_begin_catch(void) __attribute__((alias("__cxx_fatal_exception")));
extern "C" void __cxa_end_catch(void) __attribute__((alias("__cxx_fatal_exception")));
extern "C" void __cxa_get_exception_ptr(void) __attribute__((alias("__cxx_fatal_exception")));
extern "C" void __cxa_free_exception(void) __attribute__((alias("__cxx_fatal_exception")));
extern "C" void __cxa_free_dependent_exception(void) __attribute__((alias("__cxx_fatal_exception")));
extern "C" void __cxa_rethrow(void) __attribute__((alias("__cxx_fatal_exception")));
extern "C" void __cxa_throw(void) __attribute__((alias("__cxx_fatal_exception")));
extern "C" void __cxa_call_terminate(void) __attribute__((alias("__cxx_fatal_exception")));

bool std::uncaught_exception() __attribute__((alias("__cxx_fatal_exception")));

#endif // CONFIG_CXX_EXCEPTIONS
