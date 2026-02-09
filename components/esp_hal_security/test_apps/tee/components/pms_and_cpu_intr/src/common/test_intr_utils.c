/*
 * SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <assert.h>

#include "soc/soc_caps.h"
#include "soc/reg_base.h"
#include "soc/interrupts.h"
#include "soc/interrupt_matrix_reg.h"

#include "riscv/encoding.h"
#include "riscv/csr.h"
#include "riscv/rv_utils.h"

#include "esp_cpu.h"
#include "esp_rom_sys.h"

#if SOC_INT_PLIC_SUPPORTED
#include "esp_private/interrupt_plic.h"
#elif SOC_INT_CLIC_SUPPORTED
#include "esp_private/interrupt_clic.h"
#else
#include "esp_private/interrupt_intc.h"
#endif

#if CONFIG_SOC_SUPPORT_TEE_INTR_TEST
#include "test_rv_utils.h"
#include "test_cpu_intr_params.h"
#endif

/********************************** Vector Table Redirection *******************************/

void set_test_vector_table(void)
{
    extern int _test_vector_table;
    rv_utils_intr_global_disable();
    rv_utils_set_mtvec((uintptr_t)&_test_vector_table);
    rv_utils_intr_global_enable();
}

void restore_default_vector_table(void)
{
    extern int _vector_table;
    rv_utils_intr_global_disable();
    rv_utils_set_mtvec((uintptr_t)&_vector_table);
    rv_utils_intr_global_enable();
}

/********************************** Privilege Mode Switch *********************************/

void test_m2u_switch(void)
{
    int mode = esp_cpu_get_curr_privilege_level();
    assert(mode == PRV_M);

    asm volatile("ecall\n");

    mode = esp_cpu_get_curr_privilege_level();
    assert(mode == PRV_U);
}

void test_u2m_switch(void)
{
    int mode = esp_cpu_get_curr_privilege_level();
    assert(mode == PRV_U);

    asm volatile("ecall\n");

    mode = esp_cpu_get_curr_privilege_level();
    assert(mode == PRV_M);
}

/********************************** Interrupt Handler *************************************/

extern void _global_interrupt_handler(intptr_t sp, int mcause);

/* called from test_tee_vectors.S */
void _test_global_interrupt_handler(intptr_t sp, int mcause)
{
    _global_interrupt_handler(sp, mcause);
}

#if CONFIG_SOC_SUPPORT_TEE_INTR_TEST

extern int _vector_table;
extern int _mtvt_table;
extern int _test_mtvec_table;
extern int _test_mtvt_table;
extern int _test_utvec_table;
extern int _test_utvt_table;

static void test_redirect_vector_table(bool redirect)
{
    if (redirect) {
        test_rv_utils_set_xtvec((uintptr_t)&_test_mtvec_table, PRV_M);
        test_rv_utils_set_xtvt((uintptr_t)&_test_mtvt_table, PRV_M);
        test_rv_utils_set_xtvec((uintptr_t)&_test_utvec_table, PRV_U);
        test_rv_utils_set_xtvt((uintptr_t)&_test_utvt_table, PRV_U);
#if !SOC_CPU_HAS_CSR_PC
        RV_SET_CSR(mcounteren, 0x07);
#endif
    } else {
        test_rv_utils_set_xtvt((uintptr_t)&_mtvt_table, PRV_M);
        test_rv_utils_set_xtvec((uintptr_t)&_vector_table, PRV_M);
    }
}

/***************************** Miscellaneous: Interrupts (CLIC configuration save/restore) *****************************/

#define INTMTX_REG(i)         (DR_REG_INTMTX_BASE + ((i) * 4))

static uint32_t s_intr_src_map[ETS_MAX_INTR_SOURCE];
static uint32_t s_intr_clic_ctrl_reg[SOC_CPU_INTR_NUM];

static bool is_intr_ctx_saved = false;

static void test_save_intr_ctx(void)
{
    // Save and unroute all interrupt matrix sources
    for (int i = 0; i < ETS_MAX_INTR_SOURCE; i++) {
        uint32_t offs = INTMTX_REG(i);
        s_intr_src_map[i] = REG_READ(offs);
        REG_WRITE(offs, 0x00U);
    }

    // Save CLIC configuration registers
    for (int i = 0; i < SOC_CPU_INTR_NUM; i++) {
        s_intr_clic_ctrl_reg[i] = REG_READ(CLIC_INT_CTRL_REG(CLIC_EXT_INTR_NUM_OFFSET + i));
        REG_WRITE(CLIC_INT_CTRL_REG(i), 0x00U);
    }

    is_intr_ctx_saved = true;
}

static void test_restore_intr_ctx(void)
{
    if (!is_intr_ctx_saved) {
        return;
    }

    // Restore interrupt matrix sources
    for (int i = 0; i < ETS_MAX_INTR_SOURCE; i++) {
        REG_WRITE(INTMTX_REG(i), s_intr_src_map[i]);
    }

    // Restore CLIC configuration registers
    for (int i = 0; i < SOC_CPU_INTR_NUM; i++) {
        REG_WRITE(CLIC_INT_CTRL_REG(CLIC_EXT_INTR_NUM_OFFSET + i), s_intr_clic_ctrl_reg[i]);
    }

    is_intr_ctx_saved = false;
}

void test_intr_utils_init(void)
{
    test_rv_utils_intr_global_disable(PRV_M);
    test_save_intr_ctx();
    test_redirect_vector_table(true);
    test_rv_utils_intr_set_threshold(RVHAL_INTR_ENABLE_THRESH, PRV_M);
    test_rv_utils_intr_set_threshold(RVHAL_INTR_ENABLE_THRESH, PRV_U);
    test_rv_utils_intr_enable_u_mode(true);
    test_rv_utils_intr_global_enable(PRV_M);
}

void test_intr_utils_deinit(void)
{
    test_rv_utils_intr_global_disable(PRV_M);
    test_rv_utils_intr_enable_u_mode(false);
    test_redirect_vector_table(false);
    test_restore_intr_ctx();
    test_rv_utils_intr_global_enable(PRV_M);
}

/***************************** Miscellaneous: Interrupts (test allocation/freeing) *****************************/

#define TEST_INTR_NUM_MIN          (12)
#define TEST_INTR_NUM_MAX          (16)
#define TEST_INTR_NUM_COUNT        (TEST_INTR_NUM_MAX - TEST_INTR_NUM_MIN)
#define TEST_INTR_ALLOC_MASK       (((1U << TEST_INTR_NUM_COUNT) - 1) << TEST_INTR_NUM_MIN)
#define TEST_INTR_IDX(num)         ((num) - TEST_INTR_NUM_MIN)

typedef struct {
    int mode;
    int intr_src;
} test_intr_entry_t;

static uint32_t s_intr_status = 0x00U;
static test_intr_entry_t s_intr_table[TEST_INTR_NUM_COUNT];

void test_intr_alloc(int mode, int priority, int intr_src, intr_handler_t func, void *arg)
{
    int intr_num = -1;

    if (intr_src == ETS_MAX_INTR_SOURCE) {
        intr_num = TEST_INTR_NUM_PASS_IN_SEC;
    } else {
        intr_num = __builtin_ctz(~s_intr_status & TEST_INTR_ALLOC_MASK);
        if (intr_num < TEST_INTR_NUM_MIN || intr_num > TEST_INTR_NUM_MAX) {
            return;  // No free interrupts
        }

        intr_handler_set(intr_num, func, arg);
        esp_rom_route_intr_matrix(0, intr_src, intr_num);
    }

    s_intr_status |= BIT(intr_num);
    s_intr_table[TEST_INTR_IDX(intr_num)] = (test_intr_entry_t) {
        .mode = mode,
        .intr_src = intr_src,
    };

    test_rv_utils_intr_set_type(intr_num, INTR_TYPE_LEVEL);
    test_rv_utils_intr_set_priority(intr_num, priority);
    test_rv_utils_intr_set_vectored(intr_num, true);
    test_rv_utils_intr_set_mode(intr_num, mode);

    if (mode == PRV_U) {
        REG_SET_BIT(DR_REG_INTMTX_BASE + 4 * intr_src, BIT(8));
    }
    if (mode == PRV_M && intr_src == ETS_MAX_INTR_SOURCE) {
        REG_WRITE(INTERRUPT_CORE0_SIG_IDX_ASSERT_IN_SEC_REG, intr_num + CLIC_EXT_INTR_NUM_OFFSET);
    }

    test_rv_utils_intr_enable(BIT(intr_num));
}

void test_intr_free_all(void)
{
    for (int intr_num = TEST_INTR_NUM_MIN; intr_num <= TEST_INTR_NUM_MAX; intr_num++) {
        if (!(s_intr_status & BIT(intr_num))) {
            continue;
        }

        test_intr_entry_t *entry = &s_intr_table[TEST_INTR_IDX(intr_num)];
        intr_handler_set(intr_num, NULL, NULL);
        interrupt_clic_ll_route(0, entry->intr_src, ETS_INVALID_INUM);
        if (entry->mode == PRV_U) {
            REG_CLR_BIT(DR_REG_INTMTX_BASE + 4 * entry->intr_src, BIT(8));
        }
        if (entry->mode == PRV_M && entry->intr_src == ETS_MAX_INTR_SOURCE) {
            REG_WRITE(INTERRUPT_CORE0_SIG_IDX_ASSERT_IN_SEC_REG, ETS_INVALID_INUM);
        }

        test_rv_utils_intr_disable(BIT(intr_num));
        s_intr_status &= ~BIT(intr_num);
    }
}
#endif
