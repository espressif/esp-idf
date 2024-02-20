
/*
 * SPDX-FileCopyrightText: 2020-2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <ucontext.h>
#include "esp_private/eh_frame_parser.h"
#include "libunwind.h"

/**
 * @brief Index of x86 registers in `greg_t` structure.
 */
#define REG_EDI     4
#define REG_ESI     5
#define REG_EBP     6
#define REG_ESP     7
#define REG_EBX     8
#define REG_EDX     9
#define REG_ECX     10
#define REG_EAX     11
#define REG_EIP     14

/**
 * @brief Number of functions in the funs structure described below.
 */
#define FUNCTIONS_COUNT ((sizeof(funs)/sizeof(*funs)))

/**
 * @brief Number which will determine the depth of the call stack.
 * Check `main()` for more information.
 */
#define NUMBER_TO_TEST          (4)

/**
 * @brief Number of iteration for function `esp_eh_frame_generated_step`.
 */
#define NUMBER_OF_ITERATION     (2 * NUMBER_TO_TEST + 2 + 1)

/**
 * @brief Macro for testing calls to libunwind when UNW_ESUCCESS must be returned.
 */
#define UNW_CHECK(call) do { if ((err = (call)) != UNW_ESUCCESS) { \
                                printf("\e[31m\e[1mLibunwind error code %d on line %d\e[0m\r\n", err, __LINE__); \
                                exit(1); \
                             } \
                        } while(0)

/**
 * @brief Macro for testing if the given condition is true. To be used with libunwind when
 *        the result is not necessarily UNW_ESUCCESS.
 */
#define UNW_CHECK_TRUE(cond)    do { \
                                    if (!(cond)) { \
                                        printf("\e[31m\e[1mLibunwind error on line %d\e[0m\r\n", __LINE__); \
                                        exit(1); \
                                    } \
                                } while(0)

/**
 * @brief Macro for checking if a PC returned by libunwind is part of the given function
 */
#define UNW_CHECK_PC(pc, funname) do { \
                                    if (!is_pc_in_function((pc), (funname))) { \
                                        printf("\e[31m\e[1mPC %04lx should have been of function %s\e[0m\r\n", (pc), (funname)); \
                                        exit(1); \
                                    } \
                                  } while (0)

/**
 * @brief Define a simple linked list type and initialize one.
 */
struct list_t {
    uint32_t value;
    struct list_t *next;
};

static struct list_t head = { 0 };
/**
 * Few recursive functions to make the the call stack a bit more complex than a
 * single function call would give.
 */
bool is_odd(uint32_t n);
bool is_even(uint32_t n);
void browse_list(struct list_t* l);
int analyse_callstack();
int inner_function1(void);
int inner_function2(void);
void test1(void);

/**
 * @brief Structure defining a function of our program.
 * This will be used to translate the backtrace.
 */
struct functions_info {
    const char* name;
    uintptr_t start;
    uintptr_t end;  /* will be filled at runtime */
};

/**
 * @brief Structure storing the information about the
 * function that will be part of the backtrace.
 */
struct functions_info funs[] = {
    {
        .name = "browse_list",
        .start = (uintptr_t) &browse_list,
        .end = 0
    },
    {
        .name = "is_odd",
        .start = (uintptr_t) &is_odd,
        .end = 0
    },
    {
        .name = "is_even",
        .start = (uintptr_t) &is_even,
        .end = 0
    },
    {
        .name = "analyse_callstack",
        .start = (uintptr_t) &analyse_callstack,
        .end = 0
    },
    {
        .name = "inner_function1",
        .start = (uintptr_t) &inner_function1,
        .end = 0
    },
    {
        .name = "inner_function2",
        .start = (uintptr_t) &inner_function2,
        .end = 0
    },
    {
        .name = "test1",
        .start = (uintptr_t) &test1,
        .end = 0
    },
};

/**
 * @brief Test whether the address passed as PC is part of the function which
 * name is `function_name`. The global array `funs` is used.
 *
 * @param pc Program counter to test. (address in the program)
 * @param function_name Function name to check the address of.
 *
 * @return true if PC is in the function called `function_name`, false else.
 */
bool is_pc_in_function(const uint32_t pc, const char* function_name)
{
    for (uint32_t i = 0; i < FUNCTIONS_COUNT; i++) {
        const struct functions_info current_fun = funs[i];
        if (strcmp(current_fun.name, function_name) == 0) {
            return current_fun.start <= pc && pc <= current_fun.end;
        }
    }

    /* Function not found. */
    return false;
}

/**
 * @brief Number of times `esp_eh_frame_generated_step` is called.
 */
static uint32_t iteration = 1;

/**
 * @brief Override the default function called when a backtrace step is
 * generated.
 */
void esp_eh_frame_generated_step(uint32_t pc, uint32_t sp)
{
    /* The first PCs in the backtrace are calls to `browse_list()` + 2.
     * This is due to the fact that the list contains all the numbers
     * between NUMBER_TO_TEST to 0 included. Moreover, another call
     * is made when we meet the NULL pointer. */
    if (iteration > 0 && iteration <= (NUMBER_TO_TEST + 2)) {
        assert(is_pc_in_function(pc, "browse_list"));
    } else {
        /**
         * The backtrace should be:
         * - in is_odd when iteration is even.
         * - in is_even when iteration is odd.
         *
         * The backtrace finishes when the iteration reaches the end of
         * browse_list (NUMBER_TO_TEST + 2 iterations), is_even
         * (NUMBER_TO_TEST/2 calls) and is_odd (NUMBER_TO_TEST/2 calls) calls.
         */
        if (iteration >= NUMBER_OF_ITERATION) {
            return;
        } else if (iteration % 2 == 0) {
            assert(is_pc_in_function(pc, "is_odd"));
        } else {
            assert(is_pc_in_function(pc, "is_even"));
        }
    }

    /* Number of times this function has been entered. */
    iteration++;
}

/**
 * @brief Handler called when SIGSEV signal is sent to the program.
 *
 * @param signal Signal received by the program. Shall be SIGSEGV.
 * @param info Structure containing info about the error itself. Ignored.
 * @param ucontext Context of the program when the error occurred. This
 *                 is used to retrieve the CPU registers value.
 */
void signal_handler(int signal, siginfo_t *info, void *ucontext)
{
    /* Setup the execution frame as expected by the eh_frame_parser.
     * Indeed, the registers index defined in ucontext.h are NOT the same
     * the registers index DWARF is expecting. */
    ucontext_t* context = (ucontext_t*) ucontext;
    greg_t *gregset = context->uc_mcontext.gregs;
    x86ExcFrame frame = {
        .eax = gregset[REG_EAX],
        .ecx = gregset[REG_ECX],
        .edx = gregset[REG_EDX],
        .ebx = gregset[REG_EBX],
        .esp = gregset[REG_ESP],
        .ebp = gregset[REG_EBP],
        .esi = gregset[REG_ESI],
        .edi = gregset[REG_EDI],
        .eip = gregset[REG_EIP]
    };

    /* The following function will use panic_print_str and panic_print_hex
     * function to output the data.
     * Instead of replacing stdout file descriptor with a pipe, we can simply
     * replace these functions to store the data instead of printing them.
     */
    esp_eh_frame_print_backtrace(&frame);

    /* No assert has been triggered, the backtrace succeeded if the number of
     * iterations of function `esp_eh_frame_generated_step` is correct. */
    if (iteration == NUMBER_OF_ITERATION) {
        printf("\e[32m\e[1mAll tests passed \e[0m\r\n");
    } else {
        printf("\e[31m\e[1mWrong length of backtrace (%d iteration, expected %d) \e[0m\r\n",
               iteration, NUMBER_OF_ITERATION);
        exit(1);
    }

    /* Everything went fine, exit normally. */
    exit(0);
}

/**
 * @brief Browse the list passed as an argument.
 * The following function will trigger a SIGSEV signal on purpose, in order to
 * generate the backtrace.
 *
 * @param l List to browse.
 */
void browse_list(struct list_t* l)
{
    browse_list(l->next);
}

/**
 * @brief Add a number to the global list `head`.
 *
 * @param n Number to add in the list.
 */
void add_number_to_list(uint32_t n)
{
    struct list_t* l = malloc(sizeof(struct list_t));
    l->value = n;
    l->next = head.next;
    head.next = l;
}

/**
 * @brief Test if the number passed is even.
 * This function will fail, on purpose.
 *
 * @param n Number to test.
 *
 * @return true if even, false else.
 */
bool is_even(uint32_t n)
{
    add_number_to_list(n);
    if (n == 0) {
        browse_list(head.next);
        return true;
    }
    return is_odd(n - 1);
}

/**
 * @brief Test if the number passed is odd.
 * This function will fail, on purpose.
 *
 * @param n Number to test.
 *
 * @return true if odd, false else.
 */
bool is_odd(uint32_t n)
{
    add_number_to_list(n);
    if (n == 0) {
        browse_list(head.next);
        return false;
    }
    return is_even(n - 1);
}

/**
 * @brief Initialize the global `funs` array.
 */
static inline void initialize_functions_info(void)
{
    for (uint32_t i = 0; i < FUNCTIONS_COUNT; i++) {
        /* Each of the functions defined in this structure finishes
         * with the following instructions:
         * leave (0xc9)
         * ret (0xc3)
         * or
         * pop ebp (0x5d)
         * ret (0xc3)
         * Thus, we will look for these instructions. */
        uint8_t* instructions = (uint8_t*) funs[i].start;
        while ((instructions[0] != 0xc9 || instructions[1] != 0xc3) &&
                (instructions[0] != 0x5d || instructions[1] != 0xc3)) {
            instructions++;
        }
        instructions += 1;
        funs[i].end = (uintptr_t) instructions;
    }
}

/**
 * Test the eh_frame_parser for backtracing
 */
void test2(void)
{
    /* Initialize the structure holding information about the signal to override. */
    struct sigaction sig = {
        .sa_mask = 0,
        .sa_flags = SA_SIGINFO,
        .sa_restorer = NULL,
        .sa_sigaction = signal_handler
    };

    /* Override default SIGSEV signal callback. */
    int res = sigaction(SIGSEGV, &sig, NULL);
    if (res) {
        perror("Could not override SIGSEV signal");
        exit(1);
    }

    /* Trigger the segmentation fault with a complex backtrace. */
    is_even(NUMBER_TO_TEST);
}

/**
 * Test the libunwind implementation in ESP-IDF
 * Let's create some nested function calls to make unwinding more interesting.
 * Important: the stack must still be alive when analyzing it, thus it must be done
 * within the nested functions.
 */
int analyse_callstack()
{
    unw_context_t ucp = { 0 };
    unw_cursor_t cur = { 0 };
    unw_word_t pc = 0;
    int err = UNW_ESUCCESS;

    UNW_CHECK(unw_getcontext(&ucp));
    UNW_CHECK(unw_init_local(&cur, &ucp));
    UNW_CHECK(unw_get_reg(&cur, UNW_X86_EIP, &pc));
    /* This PC must be inside analyse_callstack */
    UNW_CHECK_PC(pc, "analyse_callstack");
    /* unw_step returns a positive value on success */
    UNW_CHECK_TRUE(unw_step(&cur) > 0);
    UNW_CHECK(unw_get_reg(&cur, UNW_X86_EIP, &pc));
    UNW_CHECK_PC(pc, "inner_function2");
    UNW_CHECK_TRUE(unw_step(&cur) > 0);
    UNW_CHECK(unw_get_reg(&cur, UNW_X86_EIP, &pc));
    UNW_CHECK_PC(pc, "inner_function1");
    /* unw_step returns if the frame is last one */
    UNW_CHECK_TRUE(unw_step(&cur) >= 0);
    UNW_CHECK(unw_get_reg(&cur, UNW_X86_EIP, &pc));
    UNW_CHECK_PC(pc, "test1");

    return UNW_ESUCCESS;
}

int __attribute__((noinline)) inner_function2(void)
{
    return analyse_callstack();
}

int __attribute__((noinline)) inner_function1(void)
{
    return inner_function2();
}

void __attribute__((noinline)) test1()
{
    (void) inner_function1();
}

/**
 * Call the previous tests within the main. If the first test fails, it will exit by itself.
 */
int main(int argc, char** argv)
{
    initialize_functions_info();
    test1();
    test2();
    return 0;
}
