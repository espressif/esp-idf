/*
 * SPDX-FileCopyrightText: 2011-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief Macro utilities
 *
 * Macro utilities are the public interface for C/C++ code and device tree
 * related implementation.  In general, C/C++ will include <sys/util.h>
 * instead this file directly.  For device tree implementation, this file
 * should be include instead <sys/util_internal.h>
 */

#ifndef ZEPHYR_INCLUDE_SYS_UTIL_MACROS_H_
#define ZEPHYR_INCLUDE_SYS_UTIL_MACROS_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup sys-util
 * @{
 */

/*
 * Most of the eldritch implementation details for all the macrobatics
 * below (APIs like IS_ENABLED(), COND_CODE_1(), etc.) are hidden away
 * in this file.
 */
#include <zephyr/sys/util_internal.h>

#ifndef BIT
#if defined(_ASMLANGUAGE)
#define BIT(n)  (1 << (n))
#else
/**
 * @brief Unsigned integer with bit position @p n set (signed in
 * assembly language).
 */
#define BIT(n)  (1UL << (n))
#endif
#endif

#ifndef BIT64
/** @brief 64-bit unsigned integer with bit position @p _n set. */
#define BIT64(_n) (1ULL << (_n))
#endif

/**
 * @brief Set or clear a bit depending on a boolean value
 *
 * The argument @p var is a variable whose value is written to as a
 * side effect.
 *
 * @param var Variable to be altered
 * @param bit Bit number
 * @param set if 0, clears @p bit in @p var; any other value sets @p bit
 */
#define WRITE_BIT(var, bit, set) \
    ((var) = (set) ? ((var) | BIT(bit)) : ((var) & ~BIT(bit)))

/**
 * @brief Bit mask with bits 0 through <tt>n-1</tt> (inclusive) set,
 * or 0 if @p n is 0.
 */
#define BIT_MASK(n) (BIT(n) - 1UL)

/**
 * @brief 64-bit bit mask with bits 0 through <tt>n-1</tt> (inclusive) set,
 * or 0 if @p n is 0.
 */
#define BIT64_MASK(n) (BIT64(n) - 1ULL)

/** @brief Check if a @p x is a power of two */
#define IS_POWER_OF_TWO(x) (((x) != 0U) && (((x) & ((x) - 1U)) == 0U))

/**
 * @brief Check if bits are set continuously from the specified bit
 *
 * The macro is not dependent on the bit-width.
 *
 * @param m Check whether the bits are set continuously or not.
 * @param s Specify the lowest bit for that is continuously set bits.
 */
#define IS_SHIFTED_BIT_MASK(m, s) (!(((m) >> (s)) & (((m) >> (s)) + 1U)))

/**
 * @brief Check if bits are set continuously from the LSB.
 *
 * @param m Check whether the bits are set continuously from LSB.
 */
#define IS_BIT_MASK(m) IS_SHIFTED_BIT_MASK(m, 0)

/**
 * @brief Check if bit is set in a value
 *
 * @param value Value that contain checked bit
 * @param bit Bit number
 */
#define IS_BIT_SET(value, bit) ((((value) >> (bit)) & (0x1)) != 0)

/** @brief Extract the Least Significant Bit from @p value. */
#define LSB_GET(value) ((value) & -(value))

/**
 * @brief Extract a bitfield element from @p value corresponding to
 *    the field mask @p mask.
 */
#define FIELD_GET(mask, value)  (((value) & (mask)) / LSB_GET(mask))

/**
 * @brief Prepare a bitfield element using @p value with @p mask representing
 *    its field position and width. The result should be combined
 *    with other fields using a logical OR.
 */
#define FIELD_PREP(mask, value) (((value) * LSB_GET(mask)) & (mask))

/**
 * @brief Check for macro definition in compiler-visible expressions
 *
 * This trick was pioneered in Linux as the config_enabled() macro. It
 * has the effect of taking a macro value that may be defined to "1"
 * or may not be defined at all and turning it into a literal
 * expression that can be handled by the C compiler instead of just
 * the preprocessor. It is often used with a @p CONFIG_FOO macro which
 * may be defined to 1 via Kconfig, or left undefined.
 *
 * That is, it works similarly to <tt>\#if defined(CONFIG_FOO)</tt>
 * except that its expansion is a C expression. Thus, much <tt>\#ifdef</tt>
 * usage can be replaced with equivalents like:
 *
 *     if (IS_ENABLED(CONFIG_FOO)) {
 *             do_something_with_foo
 *     }
 *
 * This is cleaner since the compiler can generate errors and warnings
 * for @p do_something_with_foo even when @p CONFIG_FOO is undefined.
 *
 * Note: Use of IS_ENABLED in a <tt>\#if</tt> statement is discouraged
 *       as it doesn't provide any benefit vs plain <tt>\#if defined()</tt>
 *
 * @param config_macro Macro to check
 * @return 1 if @p config_macro is defined to 1, 0 otherwise (including
 *         if @p config_macro is not defined)
 */
#define IS_ENABLED(config_macro) Z_IS_ENABLED1(config_macro)
/* INTERNAL: the first pass above is just to expand any existing
 * macros, we need the macro value to be e.g. a literal "1" at
 * expansion time in the next macro, not "(1)", etc... Standard
 * recursive expansion does not work.
 */

/**
 * @brief Insert code depending on whether @p _flag expands to 1 or not.
 *
 * This relies on similar tricks as IS_ENABLED(), but as the result of
 * @p _flag expansion, results in either @p _if_1_code or @p
 * _else_code is expanded.
 *
 * To prevent the preprocessor from treating commas as argument
 * separators, the @p _if_1_code and @p _else_code expressions must be
 * inside brackets/parentheses: <tt>()</tt>. These are stripped away
 * during macro expansion.
 *
 * Example:
 *
 *     COND_CODE_1(CONFIG_FLAG, (uint32_t x;), (there_is_no_flag();))
 *
 * If @p CONFIG_FLAG is defined to 1, this expands to:
 *
 *     uint32_t x;
 *
 * It expands to <tt>there_is_no_flag();</tt> otherwise.
 *
 * This could be used as an alternative to:
 *
 *     #if defined(CONFIG_FLAG) && (CONFIG_FLAG == 1)
 *     #define MAYBE_DECLARE(x) uint32_t x
 *     #else
 *     #define MAYBE_DECLARE(x) there_is_no_flag()
 *     #endif
 *
 *     MAYBE_DECLARE(x);
 *
 * However, the advantage of COND_CODE_1() is that code is resolved in
 * place where it is used, while the @p \#if method defines @p
 * MAYBE_DECLARE on two lines and requires it to be invoked again on a
 * separate line. This makes COND_CODE_1() more concise and also
 * sometimes more useful when used within another macro's expansion.
 *
 * @note @p _flag can be the result of preprocessor expansion, e.g.
 *   an expression involving <tt>NUM_VA_ARGS_LESS_1(...)</tt>.
 *   However, @p _if_1_code is only expanded if @p _flag expands
 *   to the integer literal 1. Integer expressions that evaluate
 *   to 1, e.g. after doing some arithmetic, will not work.
 *
 * @param _flag evaluated flag
 * @param _if_1_code result if @p _flag expands to 1; must be in parentheses
 * @param _else_code result otherwise; must be in parentheses
 */
#define COND_CODE_1(_flag, _if_1_code, _else_code) \
    Z_COND_CODE_1(_flag, _if_1_code, _else_code)

/**
 * @brief Like COND_CODE_1() except tests if @p _flag is 0.
 *
 * This is like COND_CODE_1(), except that it tests whether @p _flag
 * expands to the integer literal 0. It expands to @p _if_0_code if
 * so, and @p _else_code otherwise; both of these must be enclosed in
 * parentheses.
 *
 * @param _flag evaluated flag
 * @param _if_0_code result if @p _flag expands to 0; must be in parentheses
 * @param _else_code result otherwise; must be in parentheses
 * @see COND_CODE_1()
 */
#define COND_CODE_0(_flag, _if_0_code, _else_code) \
    Z_COND_CODE_0(_flag, _if_0_code, _else_code)

/**
 * @brief Insert code if @p _flag is defined and equals 1.
 *
 * Like COND_CODE_1(), this expands to @p _code if @p _flag is defined to 1;
 * it expands to nothing otherwise.
 *
 * Example:
 *
 *     IF_ENABLED(CONFIG_FLAG, (uint32_t foo;))
 *
 * If @p CONFIG_FLAG is defined to 1, this expands to:
 *
 *     uint32_t foo;
 *
 * and to nothing otherwise.
 *
 * It can be considered as a more compact alternative to:
 *
 *     #if defined(CONFIG_FLAG) && (CONFIG_FLAG == 1)
 *     uint32_t foo;
 *     #endif
 *
 * @param _flag evaluated flag
 * @param _code result if @p _flag expands to 1; must be in parentheses
 */
#define IF_ENABLED(_flag, _code) \
    COND_CODE_1(_flag, _code, ())

/**
 * @brief Insert code if @p _flag is not defined as 1.
 *
 * This expands to nothing if @p _flag is defined and equal to 1;
 * it expands to @p _code otherwise.
 *
 * Example:
 *
 *     IF_DISABLED(CONFIG_FLAG, (uint32_t foo;))
 *
 * If @p CONFIG_FLAG isn't defined or different than 1, this expands to:
 *
 *     uint32_t foo;
 *
 * and to nothing otherwise.
 *
 * IF_DISABLED does the opposite of IF_ENABLED.
 *
 * @param _flag evaluated flag
 * @param _code result if @p _flag does not expand to 1; must be in parentheses
 */
#define IF_DISABLED(_flag, _code) \
    COND_CODE_1(_flag, (), _code)

/**
 * @brief Check if a macro has a replacement expression
 *
 * If @p a is a macro defined to a nonempty value, this will return
 * true, otherwise it will return false. It only works with defined
 * macros, so an additional @p \#ifdef test may be needed in some cases.
 *
 * This macro may be used with COND_CODE_1() and COND_CODE_0() while
 * processing `__VA_ARGS__` to avoid processing empty arguments.
 *
 * Example:
 *
 *  #define EMPTY
 *  #define NON_EMPTY   1
 *  #undef  UNDEFINED
 *  IS_EMPTY(EMPTY)
 *  IS_EMPTY(NON_EMPTY)
 *  IS_EMPTY(UNDEFINED)
 *  #if defined(EMPTY) && IS_EMPTY(EMPTY) == true
 *  some_conditional_code
 *  #endif
 *
 * In above examples, the invocations of IS_EMPTY(...) return @p true,
 * @p false, and @p true; @p some_conditional_code is included.
 *
 * @param ... macro to check for emptiness (may be `__VA_ARGS__`)
 */
#define IS_EMPTY(...) Z_IS_EMPTY_(__VA_ARGS__)

/**
 * @brief Like <tt>a == b</tt>, but does evaluation and
 * short-circuiting at C preprocessor time.
 *
 * This however only works for integer literal from 0 to 4096 (literals with U suffix,
 * e.g. 0U are also included).
 *
 * Examples:
 *
 *   IS_EQ(1, 1)   -> 1
 *   IS_EQ(1U, 1U) -> 1
 *   IS_EQ(1U, 1)  -> 1
 *   IS_EQ(1, 1U)  -> 1
 *   IS_EQ(1, 0)   -> 0
 *
 * @param a Integer literal (can be with U suffix)
 * @param b Integer literal
 *
 */
#define IS_EQ(a, b) Z_IS_EQ(a, b)

/**
 * @brief Remove empty arguments from list.
 *
 * During macro expansion, `__VA_ARGS__` and other preprocessor
 * generated lists may contain empty elements, e.g.:
 *
 *  #define LIST ,a,b,,d,
 *
 * Using EMPTY to show each empty element, LIST contains:
 *
 *      EMPTY, a, b, EMPTY, d
 *
 * When processing such lists, e.g. using FOR_EACH(), all empty elements
 * will be processed, and may require filtering out.
 * To make that process easier, it is enough to invoke LIST_DROP_EMPTY
 * which will remove all empty elements.
 *
 * Example:
 *
 *  LIST_DROP_EMPTY(LIST)
 *
 * expands to:
 *
 *  a, b, d
 *
 * @param ... list to be processed
 */
#define LIST_DROP_EMPTY(...) \
    Z_LIST_DROP_FIRST(FOR_EACH(Z_LIST_NO_EMPTIES, (), __VA_ARGS__))

/**
 * @brief Macro with an empty expansion
 *
 * This trivial definition is provided for readability when a macro
 * should expand to an empty result, which e.g. is sometimes needed to
 * silence checkpatch.
 *
 * Example:
 *
 *  #define LIST_ITEM(n) , item##n
 *
 * The above would cause checkpatch to complain, but:
 *
 *  #define LIST_ITEM(n) EMPTY, item##n
 *
 * would not.
 */
#define EMPTY

/**
 * @brief Macro that expands to its argument
 *
 * This is useful in macros like @c FOR_EACH() when there is no
 * transformation required on the list elements.
 *
 * @param V any value
 */
#define IDENTITY(V) V

/**
 * @brief Get nth argument from argument list.
 *
 * @param N Argument index to fetch. Counter from 1.
 * @param ... Variable list of arguments from which one argument is returned.
 *
 * @return Nth argument.
 */
#define GET_ARG_N(N, ...) UTIL_CAT(Z_GET_ARG_, N)(__VA_ARGS__)

/**
 * @brief Strips n first arguments from the argument list.
 *
 * @param N Number of arguments to discard.
 * @param ... Variable list of arguments.
 *
 * @return argument list without N first arguments.
 */
#define GET_ARGS_LESS_N(N, ...) UTIL_CAT(Z_GET_ARGS_LESS_, N)(__VA_ARGS__)

/**
 * @brief Like <tt>a || b</tt>, but does evaluation and
 * short-circuiting at C preprocessor time.
 *
 * This is not the same as the binary @p || operator; in particular,
 * @p a should expand to an integer literal 0 or 1. However, @p b
 * can be any value.
 *
 * This can be useful when @p b is an expression that would cause a
 * build error when @p a is 1.
 */
#define UTIL_OR(a, b) COND_CODE_1(UTIL_BOOL(a), (a), (b))

/**
 * @brief Like <tt>a && b</tt>, but does evaluation and
 * short-circuiting at C preprocessor time.
 *
 * This is not the same as the binary @p &&, however; in particular,
 * @p a should expand to an integer literal 0 or 1. However, @p b
 * can be any value.
 *
 * This can be useful when @p b is an expression that would cause a
 * build error when @p a is 0.
 */
#define UTIL_AND(a, b) COND_CODE_1(UTIL_BOOL(a), (b), (0))

/**
 * @brief UTIL_INC(x) for an integer literal x from 0 to 4095 expands to an
 * integer literal whose value is x+1.
 *
 * @see UTIL_DEC(x)
 */
#define UTIL_INC(x) UTIL_PRIMITIVE_CAT(Z_UTIL_INC_, x)

/**
 * @brief UTIL_DEC(x) for an integer literal x from 0 to 4095 expands to an
 * integer literal whose value is x-1.
 *
 * @see UTIL_INC(x)
 */
#define UTIL_DEC(x) UTIL_PRIMITIVE_CAT(Z_UTIL_DEC_, x)

/**
 * @brief UTIL_X2(y) for an integer literal y from 0 to 4095 expands to an
 * integer literal whose value is 2y.
 */
#define UTIL_X2(y) UTIL_PRIMITIVE_CAT(Z_UTIL_X2_, y)

/**
 * @brief Generates a sequence of code with configurable separator.
 *
 * Example:
 *
 *     #define FOO(i, _) MY_PWM ## i
 *     { LISTIFY(PWM_COUNT, FOO, (,)) }
 *
 * The above two lines expand to:
 *
 *    { MY_PWM0 , MY_PWM1 }
 *
 * @param LEN The length of the sequence. Must be an integer literal less
 *            than 4095.
 * @param F A macro function that accepts at least two arguments:
 *          <tt>F(i, ...)</tt>. @p F is called repeatedly in the expansion.
 *          Its first argument @p i is the index in the sequence, and
 *          the variable list of arguments passed to LISTIFY are passed
 *          through to @p F.
 *
 * @param sep Separator (e.g. comma or semicolon). Must be in parentheses;
 *            this is required to enable providing a comma as separator.
 *
 * @note Calling LISTIFY with undefined arguments has undefined
 * behavior.
 */
#define LISTIFY(LEN, F, sep, ...) UTIL_CAT(Z_UTIL_LISTIFY_, LEN)(F, sep, __VA_ARGS__)

/**
 * @brief Call a macro @p F on each provided argument with a given
 *        separator between each call.
 *
 * Example:
 *
 *     #define F(x) int a##x
 *     FOR_EACH(F, (;), 4, 5, 6);
 *
 * This expands to:
 *
 *     int a4;
 *     int a5;
 *     int a6;
 *
 * @param F Macro to invoke
 * @param sep Separator (e.g. comma or semicolon). Must be in parentheses;
 *            this is required to enable providing a comma as separator.
 * @param ... Variable argument list. The macro @p F is invoked as
 *            <tt>F(element)</tt> for each element in the list.
 */
#define FOR_EACH(F, sep, ...) \
    Z_FOR_EACH(F, sep, REVERSE_ARGS(__VA_ARGS__))

/**
 * @brief Like FOR_EACH(), but with a terminator instead of a separator,
 *        and drops empty elements from the argument list
 *
 * The @p sep argument to <tt>FOR_EACH(F, (sep), a, b)</tt> is a
 * separator which is placed between calls to @p F, like this:
 *
 *     FOR_EACH(F, (sep), a, b) // F(a) sep F(b)
 *                              //               ^^^ no sep here!
 *
 * By contrast, the @p term argument to <tt>FOR_EACH_NONEMPTY_TERM(F, (term),
 * a, b)</tt> is added after each time @p F appears in the expansion:
 *
 *     FOR_EACH_NONEMPTY_TERM(F, (term), a, b) // F(a) term F(b) term
 *                                             //                ^^^^
 *
 * Further, any empty elements are dropped:
 *
 *     FOR_EACH_NONEMPTY_TERM(F, (term), a, EMPTY, b) // F(a) term F(b) term
 *
 * This is more convenient in some cases, because FOR_EACH_NONEMPTY_TERM()
 * expands to nothing when given an empty argument list, and it's
 * often cumbersome to write a macro @p F that does the right thing
 * even when given an empty argument.
 *
 * One example is when `__VA_ARGS__` may or may not be empty,
 * and the results are embedded in a larger initializer:
 *
 *     #define SQUARE(x) ((x)*(x))
 *
 *     int my_array[] = {
 *             FOR_EACH_NONEMPTY_TERM(SQUARE, (,), FOO(...))
 *             FOR_EACH_NONEMPTY_TERM(SQUARE, (,), BAR(...))
 *             FOR_EACH_NONEMPTY_TERM(SQUARE, (,), BAZ(...))
 *     };
 *
 * This is more convenient than:
 *
 * 1. figuring out whether the @p FOO, @p BAR, and @p BAZ expansions
 *    are empty and adding a comma manually (or not) between FOR_EACH()
 *    calls
 * 2. rewriting SQUARE so it reacts appropriately when "x" is empty
 *    (which would be necessary if e.g. @p FOO expands to nothing)
 *
 * @param F Macro to invoke on each nonempty element of the variable
 *          arguments
 * @param term Terminator (e.g. comma or semicolon) placed after each
 *             invocation of F. Must be in parentheses; this is required
 *             to enable providing a comma as separator.
 * @param ... Variable argument list. The macro @p F is invoked as
 *            <tt>F(element)</tt> for each nonempty element in the list.
 */
#define FOR_EACH_NONEMPTY_TERM(F, term, ...)                \
    COND_CODE_0(                            \
        /* are there zero non-empty arguments ? */      \
        NUM_VA_ARGS_LESS_1(LIST_DROP_EMPTY(__VA_ARGS__, _)),    \
        /* if so, expand to nothing */              \
        (),                         \
        /* otherwise, expand to: */             \
        (/* FOR_EACH() on nonempty elements, */     \
            FOR_EACH(F, term, LIST_DROP_EMPTY(__VA_ARGS__)) \
            /* plus a final terminator */           \
            __DEBRACKET term                \
        ))

/**
 * @brief Call macro @p F on each provided argument, with the argument's index
 *        as an additional parameter.
 *
 * This is like FOR_EACH(), except @p F should be a macro which takes two
 * arguments: <tt>F(index, variable_arg)</tt>.
 *
 * Example:
 *
 *     #define F(idx, x) int a##idx = x
 *     FOR_EACH_IDX(F, (;), 4, 5, 6);
 *
 * This expands to:
 *
 *     int a0 = 4;
 *     int a1 = 5;
 *     int a2 = 6;
 *
 * @param F Macro to invoke
 * @param sep Separator (e.g. comma or semicolon). Must be in parentheses;
 *            this is required to enable providing a comma as separator.
 * @param ... Variable argument list. The macro @p F is invoked as
 *            <tt>F(index, element)</tt> for each element in the list.
 */
#define FOR_EACH_IDX(F, sep, ...) \
    Z_FOR_EACH_IDX(F, sep, REVERSE_ARGS(__VA_ARGS__))

/**
 * @brief Call macro @p F on each provided argument, with an additional fixed
 *    argument as a parameter.
 *
 * This is like FOR_EACH(), except @p F should be a macro which takes two
 * arguments: <tt>F(variable_arg, fixed_arg)</tt>.
 *
 * Example:
 *
 *     static void func(int val, void *dev);
 *     FOR_EACH_FIXED_ARG(func, (;), dev, 4, 5, 6);
 *
 * This expands to:
 *
 *     func(4, dev);
 *     func(5, dev);
 *     func(6, dev);
 *
 * @param F Macro to invoke
 * @param sep Separator (e.g. comma or semicolon). Must be in parentheses;
 *            this is required to enable providing a comma as separator.
 * @param fixed_arg Fixed argument passed to @p F as the second macro parameter.
 * @param ... Variable argument list. The macro @p F is invoked as
 *            <tt>F(element, fixed_arg)</tt> for each element in the list.
 */
#define FOR_EACH_FIXED_ARG(F, sep, fixed_arg, ...) \
    Z_FOR_EACH_FIXED_ARG(F, sep, fixed_arg, REVERSE_ARGS(__VA_ARGS__))

/**
 * @brief Calls macro @p F for each variable argument with an index and fixed
 *        argument
 *
 * This is like the combination of FOR_EACH_IDX() with FOR_EACH_FIXED_ARG().
 *
 * Example:
 *
 *     #define F(idx, x, fixed_arg) int fixed_arg##idx = x
 *     FOR_EACH_IDX_FIXED_ARG(F, (;), a, 4, 5, 6);
 *
 * This expands to:
 *
 *     int a0 = 4;
 *     int a1 = 5;
 *     int a2 = 6;
 *
 * @param F Macro to invoke
 * @param sep Separator (e.g. comma or semicolon). Must be in parentheses;
 *            This is required to enable providing a comma as separator.
 * @param fixed_arg Fixed argument passed to @p F as the third macro parameter.
 * @param ... Variable list of arguments. The macro @p F is invoked as
 *            <tt>F(index, element, fixed_arg)</tt> for each element in
 *            the list.
 */
#define FOR_EACH_IDX_FIXED_ARG(F, sep, fixed_arg, ...) \
    Z_FOR_EACH_IDX_FIXED_ARG(F, sep, fixed_arg, REVERSE_ARGS(__VA_ARGS__))

/** @brief Reverse arguments order.
 *
 * @param ... Variable argument list.
 */
#define REVERSE_ARGS(...) \
    Z_FOR_EACH_ENGINE(Z_FOR_EACH_EXEC, (,), Z_BYPASS, _, __VA_ARGS__)

/**
 * @brief Number of arguments in the variable arguments list minus one.
 *
 * @note Supports up to 64 arguments.
 *
 * @param ... List of arguments
 * @return  Number of variadic arguments in the argument list, minus one
 */
#define NUM_VA_ARGS_LESS_1(...) \
    NUM_VA_ARGS_LESS_1_IMPL(__VA_ARGS__, 63, 62, 61, \
    60, 59, 58, 57, 56, 55, 54, 53, 52, 51,      \
    50, 49, 48, 47, 46, 45, 44, 43, 42, 41,      \
    40, 39, 38, 37, 36, 35, 34, 33, 32, 31,      \
    30, 29, 28, 27, 26, 25, 24, 23, 22, 21,      \
    20, 19, 18, 17, 16, 15, 14, 13, 12, 11,      \
    10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0, ~)

/**
 * @brief Number of arguments in the variable arguments list.
 *
 * @note Supports up to 63 arguments.
 *
 * @param ... List of arguments
 * @return  Number of variadic arguments in the argument list
 */
#define NUM_VA_ARGS(...)                                                                           \
    COND_CODE_1(IS_EMPTY(__VA_ARGS__), (0), (UTIL_INC(NUM_VA_ARGS_LESS_1(__VA_ARGS__))))

/**
 * @brief Mapping macro that pastes results together
 *
 * This is similar to FOR_EACH() in that it invokes a macro repeatedly
 * on each element of `__VA_ARGS__`. However, unlike FOR_EACH(),
 * MACRO_MAP_CAT() pastes the results together into a single token.
 *
 * For example, with this macro FOO:
 *
 *     #define FOO(x) item_##x##_
 *
 * <tt>MACRO_MAP_CAT(FOO, a, b, c),</tt> expands to the token:
 *
 *     item_a_item_b_item_c_
 *
 * @param ... Macro to expand on each argument, followed by its
 *            arguments. (The macro should take exactly one argument.)
 * @return The results of expanding the macro on each argument, all pasted
 *         together
 */
#define MACRO_MAP_CAT(...) MACRO_MAP_CAT_(__VA_ARGS__)

/**
 * @brief Mapping macro that pastes a fixed number of results together
 *
 * Similar to @ref MACRO_MAP_CAT(), but expects a fixed number of
 * arguments. If more arguments are given than are expected, the rest
 * are ignored.
 *
 * @param N   Number of arguments to map
 * @param ... Macro to expand on each argument, followed by its
 *            arguments. (The macro should take exactly one argument.)
 * @return The results of expanding the macro on each argument, all pasted
 *         together
 */
#define MACRO_MAP_CAT_N(N, ...) MACRO_MAP_CAT_N_(N, __VA_ARGS__)

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_SYS_UTIL_MACROS_H_ */
