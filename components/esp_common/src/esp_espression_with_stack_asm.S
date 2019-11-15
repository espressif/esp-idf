#include    <xtensa/coreasm.h>
#include    <freertos/xtensa_context.h>

    .text
/**
 * extern void switch_stack_enter(portSTACK_TYPE *stack, portSTACK_TYPE *backup_stack);
 */
    .globl     switch_stack_enter
    .type       switch_stack_enter,@function
    .align      4
switch_stack_enter:
    entry   sp, 0x10
    mov   a4, a1
    s32i   a4, a3, 0 /* on a3 there is a safe place to save the current stack */
    l32i   a4, a2, 0 /* obtains the user allocated stack buffer */
    mov   a1, a4     /* sp register now contains caller specified stack */
    retw

/**
 * extern void switch_stack_exit(portSTACK_TYPE *backup_stack);
 */
    .globl  switch_stack_exit
    .type   switch_stack_exit,@function
    .align  4
switch_stack_exit:
    entry   sp, 0x10
    l32i a4, a2, 0  /* recover the original task stack */
    mov a1, a4      /* put it on sp register again */
    retw