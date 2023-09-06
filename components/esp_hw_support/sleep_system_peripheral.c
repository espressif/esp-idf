/*
 * SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stddef.h>
#include <string.h>

#include "sdkconfig.h"
#include "soc/soc_caps.h"

#include "esp_sleep.h"
#include "esp_log.h"
#include "esp_check.h"

#include "esp_private/startup_internal.h"
#include "esp_private/sleep_retention.h"
#include "esp_regdma.h"

#include "soc/uart_reg.h"
#include "soc/systimer_reg.h"
#include "soc/timer_group_reg.h"
#include "soc/spi_mem_reg.h"
#include "soc/hp_system_reg.h"
#include "soc/tee_reg.h"
#include "soc/hp_apm_reg.h"
#include "soc/gpio_reg.h"
#include "soc/io_mux_reg.h"
#include "soc/interrupt_matrix_reg.h"
#include "hal/mwdt_ll.h"

static __attribute__((unused)) const char *TAG = "sleep_sys_periph";

#define SLEEP_RETENTION_PERIPHERALS_PRIORITY_DEFAULT    (REGDMA_LINK_PRI_6)

esp_err_t sleep_sys_periph_intr_matrix_retention_init(void)
{
    #define N_REGS_INTR_MATRIX()    (((INTMTX_CORE0_CLOCK_GATE_REG - DR_REG_INTERRUPT_MATRIX_BASE) / 4) + 1)

    const static sleep_retention_entries_config_t intr_matrix_regs_retention[] = {
        [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_INTMTX_LINK(0), DR_REG_INTERRUPT_MATRIX_BASE, DR_REG_INTERRUPT_MATRIX_BASE, N_REGS_INTR_MATRIX(), 0, 0), .owner = ENTRY(0) | ENTRY(2) }  /* intr matrix */
    };

    esp_err_t err = sleep_retention_entries_create(intr_matrix_regs_retention, ARRAY_SIZE(intr_matrix_regs_retention), REGDMA_LINK_PRI_5, SLEEP_RETENTION_MODULE_INTR_MATRIX);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for digital peripherals (Interrupt matrix) retention");
    ESP_LOGI(TAG, "Interrupt Matrix sleep retention initialization");
    return ESP_OK;
}

esp_err_t sleep_sys_periph_hp_system_retention_init(void)
{
    #define N_REGS_HP_SYSTEM()      (((HP_SYSTEM_MEM_TEST_CONF_REG - DR_REG_HP_SYSTEM_BASE) / 4) + 1)

    const static sleep_retention_entries_config_t hp_system_regs_retention[] = {
        [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_HPSYS_LINK(0), DR_REG_HP_SYSTEM_BASE, DR_REG_HP_SYSTEM_BASE, N_REGS_HP_SYSTEM(), 0, 0), .owner = ENTRY(0) | ENTRY(2) }  /* hp system */
    };

    esp_err_t err = sleep_retention_entries_create(hp_system_regs_retention, ARRAY_SIZE(hp_system_regs_retention), REGDMA_LINK_PRI_5, SLEEP_RETENTION_MODULE_HP_SYSTEM);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for digital peripherals (HP system) retention");
    ESP_LOGI(TAG, "HP System sleep retention initialization");
    return ESP_OK;
}

esp_err_t sleep_sys_periph_tee_apm_retention_init(void)
{
    #define N_REGS_TEE()    (((TEE_CLOCK_GATE_REG - DR_REG_TEE_BASE) / 4) + 1)
    #define N_REGS_APM()    (((HP_APM_CLOCK_GATE_REG - DR_REG_HP_APM_BASE) / 4) + 1)

    const static sleep_retention_entries_config_t tee_apm_regs_retention[] = {
        [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_TEEAPM_LINK(0), DR_REG_HP_APM_BASE, DR_REG_HP_APM_BASE, N_REGS_APM(), 0, 0), .owner = ENTRY(0) | ENTRY(2) }, /* apm */
        [1] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_TEEAPM_LINK(1), DR_REG_TEE_BASE,    DR_REG_TEE_BASE,    N_REGS_TEE(), 0, 0), .owner = ENTRY(0) | ENTRY(2) }  /* tee */
    };

    esp_err_t err = sleep_retention_entries_create(tee_apm_regs_retention, ARRAY_SIZE(tee_apm_regs_retention), REGDMA_LINK_PRI_4, SLEEP_RETENTION_MODULE_TEE_APM);
    if (err == ESP_OK) {
        const static sleep_retention_entries_config_t regs_highpri_retention[] = {
            [0] = { .config = REGDMA_LINK_WRITE_INIT(REGDMA_TEEAPM_LINK(2), TEE_M4_MODE_CTRL_REG, 0x0, 0xffffffff, 1, 0), .owner = ENTRY(2) }
        };
        err = sleep_retention_entries_create(regs_highpri_retention, ARRAY_SIZE(regs_highpri_retention), REGDMA_LINK_PRI_2, SLEEP_RETENTION_MODULE_TEE_APM);
    }
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for digital peripherals (TEE/APM) retention");
    ESP_LOGI(TAG, "TEE/APM sleep retention initialization");
    return ESP_OK;
}

esp_err_t sleep_sys_periph_uart0_retention_init(void)
{
    #define N_REGS_UART()   (((UART_ID_REG(0) - UART_INT_RAW_REG(0)) / 4) + 1)

    const static sleep_retention_entries_config_t uart_regs_retention[] = {
        [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_UART_LINK(0x00), UART_INT_RAW_REG(0),       UART_INT_RAW_REG(0), N_REGS_UART(),     0, 0), .owner = ENTRY(0) | ENTRY(2) }, /* uart */
        /* Note: uart register should set update reg to make the configuration take effect */
        [1] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_UART_LINK(0x01), UART_REG_UPDATE_REG(0), UART_REG_UPDATE,  UART_REG_UPDATE_M, 1, 0), .owner = ENTRY(0) | ENTRY(2) },
        [2] = { .config = REGDMA_LINK_WAIT_INIT      (REGDMA_UART_LINK(0x02), UART_REG_UPDATE_REG(0), 0x0,              UART_REG_UPDATE_M, 1, 0), .owner = ENTRY(0) | ENTRY(2) }
    };

    esp_err_t err = sleep_retention_entries_create(uart_regs_retention, ARRAY_SIZE(uart_regs_retention), REGDMA_LINK_PRI_5, SLEEP_RETENTION_MODULE_UART0);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for digital peripherals (UART) retention");
    ESP_LOGI(TAG, "UART sleep retention initialization");
    return ESP_OK;
}

esp_err_t sleep_sys_periph_tg0_retention_init(void)
{
    #define N_REGS_TG()     (((TIMG_REGCLK_REG(0) - REG_TIMG_BASE(0)) / 4) + 1)

    const static sleep_retention_entries_config_t tg_regs_retention[] = {
        /*Timer group0 backup. T0_wdt should get of write project firstly. wdt used by RTOS.*/
        [0] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_TIMG_LINK(0x00), TIMG_WDTWPROTECT_REG(0), TIMG_WDT_WKEY_VALUE,     TIMG_WDT_WKEY_M,           1, 0), .owner = ENTRY(0) | ENTRY(2) }, /* TG0 */
        [1] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_TIMG_LINK(0x01), REG_TIMG_BASE(0),        REG_TIMG_BASE(0),        N_REGS_TG(),               0, 0), .owner = ENTRY(0) | ENTRY(2) },
        [2] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_TIMG_LINK(0x02), TIMG_WDTWPROTECT_REG(0), TIMG_WDT_WKEY_VALUE,     TIMG_WDT_WKEY_M,           1, 0), .owner = ENTRY(0) | ENTRY(2) },
        [3] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_TIMG_LINK(0x03), TIMG_WDTCONFIG0_REG(0),  TIMG_WDT_CONF_UPDATE_EN, TIMG_WDT_CONF_UPDATE_EN_M, 1, 0), .owner = ENTRY(0) | ENTRY(2) },
        [4] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_TIMG_LINK(0x04), TIMG_T0UPDATE_REG(0),    TIMG_T0_UPDATE,          TIMG_T0_UPDATE_M,          0, 1), .owner = ENTRY(0) | ENTRY(2) },
        [5] = { .config = REGDMA_LINK_WAIT_INIT      (REGDMA_TIMG_LINK(0x05), TIMG_T0UPDATE_REG(0),    0x0,                     TIMG_T0_UPDATE_M,          0, 1), .owner = ENTRY(0) | ENTRY(2) },
        [6] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_TIMG_LINK(0x06), TIMG_T0LO_REG(0),        TIMG_T0LOADLO_REG(0),    2,                         0, 0), .owner = ENTRY(0) | ENTRY(2) },
        [7] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_TIMG_LINK(0x07), TIMG_T0LOAD_REG(0),      0x1,                     TIMG_T0_LOAD_M,            1, 0), .owner = ENTRY(0) | ENTRY(2) }
    };

    esp_err_t err = sleep_retention_entries_create(tg_regs_retention, ARRAY_SIZE(tg_regs_retention), SLEEP_RETENTION_PERIPHERALS_PRIORITY_DEFAULT, SLEEP_RETENTION_MODULE_TG0);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for digital peripherals (Timer Group) retention");
    ESP_LOGI(TAG, "Timer Group sleep retention initialization");
    return ESP_OK;
}

esp_err_t sleep_sys_periph_iomux_retention_init(void)
{
#if CONFIG_IDF_TARGET_ESP32C6
    #define N_REGS_IOMUX_0()    (((PERIPHS_IO_MUX_SPID_U - REG_IO_MUX_BASE) / 4) + 1)
    #define N_REGS_IOMUX_1()    (((GPIO_FUNC34_OUT_SEL_CFG_REG - GPIO_FUNC0_OUT_SEL_CFG_REG) / 4) + 1)
    #define N_REGS_IOMUX_2()    (((GPIO_FUNC124_IN_SEL_CFG_REG - GPIO_STATUS_NEXT_REG) / 4) + 1)
    #define N_REGS_IOMUX_3()    (((GPIO_PIN34_REG - DR_REG_GPIO_BASE) / 4) + 1)
#elif CONFIG_IDF_TARGET_ESP32H2
    #define N_REGS_IOMUX_0()    (((PERIPHS_IO_MUX_SPID_U - REG_IO_MUX_BASE) / 4) + 1)
    #define N_REGS_IOMUX_1()    (((GPIO_FUNC31_OUT_SEL_CFG_REG - GPIO_FUNC0_OUT_SEL_CFG_REG) / 4) + 1)
    #define N_REGS_IOMUX_2()    (((GPIO_FUNC124_IN_SEL_CFG_REG - GPIO_STATUS_NEXT_REG) / 4) + 1)
    #define N_REGS_IOMUX_3()    (((GPIO_PIN31_REG - DR_REG_GPIO_BASE) / 4) + 1)
#endif
    const static sleep_retention_entries_config_t iomux_regs_retention[] = {
        [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_IOMUX_LINK(0x00), REG_IO_MUX_BASE,            REG_IO_MUX_BASE,            N_REGS_IOMUX_0(), 0, 0), .owner = ENTRY(0) | ENTRY(2) }, /* io_mux */
        [1] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_IOMUX_LINK(0x01), GPIO_FUNC0_OUT_SEL_CFG_REG, GPIO_FUNC0_OUT_SEL_CFG_REG, N_REGS_IOMUX_1(), 0, 0), .owner = ENTRY(0) | ENTRY(2) },
        [2] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_IOMUX_LINK(0x02), GPIO_STATUS_NEXT_REG,       GPIO_STATUS_NEXT_REG,       N_REGS_IOMUX_2(), 0, 0), .owner = ENTRY(0) | ENTRY(2) },
        [3] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_IOMUX_LINK(0x03), DR_REG_GPIO_BASE,           DR_REG_GPIO_BASE,           N_REGS_IOMUX_3(), 0, 0), .owner = ENTRY(0) | ENTRY(2) }
    };

    esp_err_t err = sleep_retention_entries_create(iomux_regs_retention, ARRAY_SIZE(iomux_regs_retention), SLEEP_RETENTION_PERIPHERALS_PRIORITY_DEFAULT, SLEEP_RETENTION_MODULE_IOMUX);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for digital peripherals (IO Matrix) retention");
    ESP_LOGI(TAG, "IO Matrix sleep retention initialization");
    return ESP_OK;
}

esp_err_t sleep_sys_periph_spimem_retention_init(void)
{
    #define N_REGS_SPI1_MEM_0()     (((SPI_MEM_SPI_SMEM_DDR_REG(1) - REG_SPI_MEM_BASE(1)) / 4) + 1)
    #define N_REGS_SPI1_MEM_1()     (((SPI_MEM_SPI_SMEM_AC_REG(1) - SPI_MEM_SPI_FMEM_PMS0_ATTR_REG(1)) / 4) + 1)
    #define N_REGS_SPI1_MEM_2()     (1)
    #define N_REGS_SPI1_MEM_3()     (((SPI_MEM_DATE_REG(1) - SPI_MEM_MMU_POWER_CTRL_REG(1)) / 4) + 1)

    #define N_REGS_SPI0_MEM_0()     (((SPI_MEM_SPI_SMEM_DDR_REG(0) - REG_SPI_MEM_BASE(0)) / 4) + 1)
    #define N_REGS_SPI0_MEM_1()     (((SPI_MEM_SPI_SMEM_AC_REG(0) - SPI_MEM_SPI_FMEM_PMS0_ATTR_REG(0)) / 4) + 1)
    #define N_REGS_SPI0_MEM_2()     (1)
    #define N_REGS_SPI0_MEM_3()     (((SPI_MEM_DATE_REG(0) - SPI_MEM_MMU_POWER_CTRL_REG(0)) / 4) + 1)

    const static sleep_retention_entries_config_t spimem_regs_retention[] = {
        /* Note: SPI mem should not to write mmu SPI_MEM_MMU_ITEM_CONTENT_REG and SPI_MEM_MMU_ITEM_INDEX_REG */
        [0] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x00), REG_SPI_MEM_BASE(1),               REG_SPI_MEM_BASE(1),               N_REGS_SPI1_MEM_0(), 0, 0), .owner = ENTRY(0) | ENTRY(2) }, /* spi1_mem */
        [1] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x01), SPI_MEM_SPI_FMEM_PMS0_ATTR_REG(1), SPI_MEM_SPI_FMEM_PMS0_ATTR_REG(1), N_REGS_SPI1_MEM_1(), 0, 0), .owner = ENTRY(0) | ENTRY(2) },
        [2] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x02), SPI_MEM_CLOCK_GATE_REG(1),         SPI_MEM_CLOCK_GATE_REG(1),         N_REGS_SPI1_MEM_2(), 0, 0), .owner = ENTRY(0) | ENTRY(2) },
        [3] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x03), SPI_MEM_MMU_POWER_CTRL_REG(1),           SPI_MEM_MMU_POWER_CTRL_REG(1),           N_REGS_SPI1_MEM_3(), 0, 0), .owner = ENTRY(0) | ENTRY(2) },

        /* Note: SPI mem should not to write mmu SPI_MEM_MMU_ITEM_CONTENT_REG and SPI_MEM_MMU_ITEM_INDEX_REG */
        [4] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x04), REG_SPI_MEM_BASE(0),               REG_SPI_MEM_BASE(0),               N_REGS_SPI0_MEM_0(), 0, 0), .owner = ENTRY(0) | ENTRY(2) }, /* spi0_mem */
        [5] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x05), SPI_MEM_SPI_FMEM_PMS0_ATTR_REG(0), SPI_MEM_SPI_FMEM_PMS0_ATTR_REG(0), N_REGS_SPI0_MEM_1(), 0, 0), .owner = ENTRY(0) | ENTRY(2) },
        [6] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x06), SPI_MEM_CLOCK_GATE_REG(0),         SPI_MEM_CLOCK_GATE_REG(0),         N_REGS_SPI0_MEM_2(), 0, 0), .owner = ENTRY(0) | ENTRY(2) },
        [7] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SPIMEM_LINK(0x07), SPI_MEM_MMU_POWER_CTRL_REG(0),           SPI_MEM_MMU_POWER_CTRL_REG(0),           N_REGS_SPI0_MEM_3(), 0, 0), .owner = ENTRY(0) | ENTRY(2) }
    };

    esp_err_t err = sleep_retention_entries_create(spimem_regs_retention, ARRAY_SIZE(spimem_regs_retention), SLEEP_RETENTION_PERIPHERALS_PRIORITY_DEFAULT, SLEEP_RETENTION_MODULE_SPIMEM);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for digital peripherals (SPI mem) retention");
    ESP_LOGI(TAG, "SPI Mem sleep retention initialization");
    return ESP_OK;
}

esp_err_t sleep_sys_periph_systimer_retention_init(void)
{
    #define N_REGS_SYSTIMER_0()     (((SYSTIMER_TARGET2_CONF_REG - SYSTIMER_TARGET0_HI_REG) / 4) + 1)

    const static sleep_retention_entries_config_t systimer_regs_retention[] = {
        [0]  = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_SYSTIMER_LINK(0x00), SYSTIMER_UNIT0_OP_REG,       SYSTIMER_TIMER_UNIT0_UPDATE_M,    SYSTIMER_TIMER_UNIT0_UPDATE_M,    0, 1), .owner = ENTRY(0) | ENTRY(2) }, /* Systimer */
        [1]  = { .config = REGDMA_LINK_WAIT_INIT      (REGDMA_SYSTIMER_LINK(0x01), SYSTIMER_UNIT0_OP_REG,       SYSTIMER_TIMER_UNIT0_VALUE_VALID, SYSTIMER_TIMER_UNIT0_VALUE_VALID, 0, 1), .owner = ENTRY(0) | ENTRY(2) },
        [2]  = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SYSTIMER_LINK(0x02), SYSTIMER_UNIT0_VALUE_HI_REG, SYSTIMER_UNIT0_LOAD_HI_REG,       2,                                0, 0), .owner = ENTRY(0) | ENTRY(2) },
        [3]  = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_SYSTIMER_LINK(0x03), SYSTIMER_UNIT0_LOAD_REG,     SYSTIMER_TIMER_UNIT0_LOAD_M,      SYSTIMER_TIMER_UNIT0_LOAD_M,      1, 0), .owner = ENTRY(0) | ENTRY(2) },

        [4]  = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_SYSTIMER_LINK(0x04), SYSTIMER_UNIT1_OP_REG,       SYSTIMER_TIMER_UNIT1_UPDATE_M,    SYSTIMER_TIMER_UNIT1_UPDATE_M,    0, 1), .owner = ENTRY(0) | ENTRY(2) },
        [5]  = { .config = REGDMA_LINK_WAIT_INIT      (REGDMA_SYSTIMER_LINK(0x05), SYSTIMER_UNIT1_OP_REG,       SYSTIMER_TIMER_UNIT1_VALUE_VALID, SYSTIMER_TIMER_UNIT1_VALUE_VALID, 0, 1), .owner = ENTRY(0) | ENTRY(2) },
        [6]  = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SYSTIMER_LINK(0x06), SYSTIMER_UNIT1_VALUE_HI_REG, SYSTIMER_UNIT1_LOAD_HI_REG,       2,                                0, 0), .owner = ENTRY(0) | ENTRY(2) },
        [7]  = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_SYSTIMER_LINK(0x07), SYSTIMER_UNIT1_LOAD_REG,     SYSTIMER_TIMER_UNIT1_LOAD_M,      SYSTIMER_TIMER_UNIT1_LOAD_M,      1, 0), .owner = ENTRY(0) | ENTRY(2) },

        [8]  = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SYSTIMER_LINK(0x08), SYSTIMER_TARGET0_HI_REG,     SYSTIMER_TARGET0_HI_REG,          N_REGS_SYSTIMER_0(),              0, 0), .owner = ENTRY(0) | ENTRY(2) }, /* Systimer target value & period */

        [9]  = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_SYSTIMER_LINK(0x09), SYSTIMER_COMP0_LOAD_REG,     SYSTIMER_TIMER_COMP0_LOAD,        SYSTIMER_TIMER_COMP0_LOAD,        1, 0), .owner = ENTRY(0) | ENTRY(2) },
        [10] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_SYSTIMER_LINK(0x0a), SYSTIMER_COMP1_LOAD_REG,     SYSTIMER_TIMER_COMP1_LOAD,        SYSTIMER_TIMER_COMP1_LOAD,        1, 0), .owner = ENTRY(0) | ENTRY(2) },
        [11] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_SYSTIMER_LINK(0x0b), SYSTIMER_COMP2_LOAD_REG,     SYSTIMER_TIMER_COMP2_LOAD,        SYSTIMER_TIMER_COMP2_LOAD,        1, 0), .owner = ENTRY(0) | ENTRY(2) },

        [12] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_SYSTIMER_LINK(0x0c), SYSTIMER_TARGET0_CONF_REG,   0,                                SYSTIMER_TARGET0_PERIOD_MODE_M,   1, 0), .owner = ENTRY(0) | ENTRY(2) },
        [13] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_SYSTIMER_LINK(0x0d), SYSTIMER_TARGET0_CONF_REG,   SYSTIMER_TARGET0_PERIOD_MODE_M,   SYSTIMER_TARGET0_PERIOD_MODE_M,   1, 0), .owner = ENTRY(0) | ENTRY(2) },

        [14] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_SYSTIMER_LINK(0x0e), SYSTIMER_TARGET1_CONF_REG,   0,                                SYSTIMER_TARGET1_PERIOD_MODE_M,   1, 0), .owner = ENTRY(0) | ENTRY(2) },
        [15] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_SYSTIMER_LINK(0x0f), SYSTIMER_TARGET1_CONF_REG,   SYSTIMER_TARGET1_PERIOD_MODE_M,   SYSTIMER_TARGET1_PERIOD_MODE_M,   1, 0), .owner = ENTRY(0) | ENTRY(2) },

        [16] = { .config = REGDMA_LINK_WRITE_INIT     (REGDMA_SYSTIMER_LINK(0x10), SYSTIMER_TARGET2_CONF_REG,   0,                                SYSTIMER_TARGET2_PERIOD_MODE_M,   1, 0), .owner = ENTRY(0) | ENTRY(2) },

        [17] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SYSTIMER_LINK(0x11), SYSTIMER_CONF_REG,           SYSTIMER_CONF_REG,                1,                                0, 0), .owner = ENTRY(0) | ENTRY(2) }, /* Systimer work enable */
        [18] = { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_SYSTIMER_LINK(0x12), SYSTIMER_INT_ENA_REG,        SYSTIMER_INT_ENA_REG,             1,                                0, 0), .owner = ENTRY(0) | ENTRY(2) }  /* Systimer intr enable */
    };

    esp_err_t err = sleep_retention_entries_create(systimer_regs_retention, ARRAY_SIZE(systimer_regs_retention), SLEEP_RETENTION_PERIPHERALS_PRIORITY_DEFAULT, SLEEP_RETENTION_MODULE_SYSTIMER);
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for digital peripherals (SysTimer) retention");
    ESP_LOGI(TAG, "SysTimer sleep retention initialization");
    return ESP_OK;
}

esp_err_t sleep_sys_periph_retention_init(void)
{
    esp_err_t err;
    err = sleep_sys_periph_intr_matrix_retention_init();
    if(err) goto error;
    err = sleep_sys_periph_hp_system_retention_init();
    if(err) goto error;
    err = sleep_sys_periph_tee_apm_retention_init();
    if(err) goto error;
    err = sleep_sys_periph_uart0_retention_init();
    if(err) goto error;
    err = sleep_sys_periph_tg0_retention_init();
    if(err) goto error;
    err = sleep_sys_periph_iomux_retention_init();
    if(err) goto error;
    err = sleep_sys_periph_spimem_retention_init();
    if(err) goto error;
    err = sleep_sys_periph_systimer_retention_init();

error:
    return err;
}

bool peripheral_domain_pd_allowed(void)
{
    const uint32_t modules = sleep_retention_get_modules();
    const uint32_t mask = (const uint32_t) (
            SLEEP_RETENTION_MODULE_INTR_MATRIX | \
            SLEEP_RETENTION_MODULE_HP_SYSTEM   | \
            SLEEP_RETENTION_MODULE_TEE_APM     | \
            SLEEP_RETENTION_MODULE_UART0       | \
            SLEEP_RETENTION_MODULE_TG0         | \
            SLEEP_RETENTION_MODULE_IOMUX       | \
            SLEEP_RETENTION_MODULE_SPIMEM      | \
            SLEEP_RETENTION_MODULE_SYSTIMER);
    return ((modules & mask) == mask);
}

#if CONFIG_PM_POWER_DOWN_PERIPHERAL_IN_LIGHT_SLEEP
ESP_SYSTEM_INIT_FN(sleep_sys_periph_startup_init, BIT(0), 107)
{
    sleep_sys_periph_retention_init();
    return ESP_OK;
}
#endif
