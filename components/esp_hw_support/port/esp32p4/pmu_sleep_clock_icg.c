/*
 * SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "esp_pmu.h"
#include "esp_check.h"
#include "soc/hp_sys_clkrst_reg.h"
#include "esp_private/sleep_retention.h"
#include "esp_private/startup_internal.h"

ESP_LOG_ATTR_TAG(TAG, "sleep_clock");

#define SOC_CLK_EN(peri)    (HP_SYS_CLKRST_REG_ ## peri ## _CLK_EN)
#define SOC_CLK_MASK(peri)  (HP_SYS_CLKRST_REG_ ## peri ## _CLK_EN_M)

#define PERI_CLK_EN(peri)   (HP_SYS_CLKRST_REG_ ## peri ## _CLK_EN)
#define PERI_CLK_MASK(peri) (HP_SYS_CLKRST_REG_ ## peri ## _CLK_EN_M)


#define MASK(m, bit)    m ## _MASK(bit)

#define MASK_1(m, a)                    MASK(m, a)
#define MASK_2(m, a, b)                 MASK(m, a) | MASK(m, b)
#define MASK_3(m, a, b, c)              MASK(m, a) | MASK_2(m, b, c)
#define MASK_4(m, a, b, c, d)           MASK(m, a) | MASK_3(m, b, c, d)
#define MASK_5(m, a, b, c, d, e)        MASK(m, a) | MASK_4(m, b, c, d, e)
#define MASK_6(m, a, b, c, d, e, ...)   MASK_5(m, a, b, c, d, e) | MASK_1(m, __VA_ARGS__)
#define MASK_7(m, a, b, c, d, e, ...)   MASK_5(m, a, b, c, d, e) | MASK_2(m, __VA_ARGS__)
#define MASK_8(m, a, b, c, d, e, ...)   MASK_5(m, a, b, c, d, e) | MASK_3(m, __VA_ARGS__)
#define MASK_9(m, a, b, c, d, e, ...)   MASK_5(m, a, b, c, d, e) | MASK_4(m, __VA_ARGS__)

#define MASK_N(n)                   MASK_##n
#define MASK_DISPATCH(m, n, ...)    MASK_N(n)(m, __VA_ARGS__)

#define MASK_BITS(m, ...)           MASK_DISPATCH(m, ESP_VA_NARG(__VA_ARGS__), __VA_ARGS__)


#define SOC_CLK_MASK_BITS(...)      MASK_BITS(SOC_CLK, __VA_ARGS__)
#define PERI_CLK_MASK_BITS(...)     MASK_BITS(PERI_CLK, __VA_ARGS__)


#define SOC_CLK_CTRL2_REG_MASK0     SOC_CLK_MASK_BITS( RMT_SYS, HP_CLKRST_APB, SYSREG_APB, ICM_APB, INTRMTX_APB, ADC_APB, UHCI_APB )
#define SOC_CLK_CTRL2_REG_MASK1     SOC_CLK_MASK_BITS( UART0_APB, UART1_APB, UART2_APB, UART3_APB, UART4_APB, I2C0_APB, I2C1_APB )
#define SOC_CLK_CTRL2_REG_MASK2     SOC_CLK_MASK_BITS( I2S0_APB, I2S1_APB, I2S2_APB, I3C_MST_APB, I3C_SLV_APB, GPSPI2_APB, GPSPI3_APB )
#define SOC_CLK_CTRL2_REG_MASK3     SOC_CLK_MASK_BITS( TIMERGRP0_APB, TIMERGRP1_APB, SYSTIMER_APB, TWAI0_APB, TWAI1_APB, TWAI2_APB )
#define SOC_CLK_CTRL2_REG_MASK4     SOC_CLK_MASK_BITS( MCPWM0_APB, MCPWM1_APB, USB_DEVICE_APB, PCNT_APB, PARLIO_APB )
#define SOC_CLK_CTRL2_REG_MASK()    SOC_CLK_CTRL2_REG_MASK0 | SOC_CLK_CTRL2_REG_MASK1 | SOC_CLK_CTRL2_REG_MASK2 | SOC_CLK_CTRL2_REG_MASK3 | SOC_CLK_CTRL2_REG_MASK4
#define SOC_CLK_CTRL3_REG_MASK()    SOC_CLK_MASK_BITS( IOMUX_APB, LEDC_APB, LCDCAM_APB, ETM_APB)

#define PERI_CLK_CTRL00_REG_MASK()  PERI_CLK_MASK_BITS( FLASH_PLL, FLASH_CORE, PSRAM_PLL, PSRAM_CORE, PAD_EMAC_REF, EMAC_RMII, EMAC_RX )
#define PERI_CLK_CTRL01_REG_MASK()  PERI_CLK_MASK_BITS( EMAC_TX, EMAC_PTP_REF, EMAC_UNUSED0, EMAC_UNUSED1, SDIO_LS )
#define PERI_CLK_CTRL02_REG_MASK()  PERI_CLK_MASK_BITS( SDIO_LS_SLF, SDIO_LS_DRV, SDIO_LS_SAM )
#define PERI_CLK_CTRL03_REG_MASK()  PERI_CLK_MASK_BITS( MIPI_DSI_DPHY_CFG, MIPI_CSI_DPHY_CFG) | HP_SYS_CLKRST_REG_MIPI_DSI_DPHY_PLL_REFCLK_EN_M | HP_SYS_CLKRST_REG_MIPI_DSI_DPICLK_EN_M
#define PERI_CLK_CTRL10_REG_MASK()  PERI_CLK_MASK_BITS( I2C0, I2C1 )
#define PERI_CLK_CTRL11_REG_MASK()  PERI_CLK_MASK_BITS( I2S0_RX )
#define PERI_CLK_CTRL13_REG_MASK()  PERI_CLK_MASK_BITS( I2S0_TX )
#define PERI_CLK_CTRL14_REG_MASK()  PERI_CLK_MASK_BITS( I2S1_RX )
#define PERI_CLK_CTRL15_REG_MASK()  PERI_CLK_MASK_BITS( I2S1_TX )
#define PERI_CLK_CTRL17_REG_MASK()  PERI_CLK_MASK_BITS( I2S2_RX )
#define PERI_CLK_CTRL18_REG_MASK()  PERI_CLK_MASK_BITS( I2S2_TX )
#define PERI_CLK_CTRL19_REG_MASK()  PERI_CLK_MASK_BITS( LCD )
#define PERI_CLK_CTRL110_REG_MASK() PERI_CLK_MASK_BITS( UART0 )
#define PERI_CLK_CTRL111_REG_MASK() PERI_CLK_MASK_BITS( UART1 )
#define PERI_CLK_CTRL112_REG_MASK() PERI_CLK_MASK_BITS( UART2 )
#define PERI_CLK_CTRL113_REG_MASK() PERI_CLK_MASK_BITS( UART3 )
#define PERI_CLK_CTRL114_REG_MASK() PERI_CLK_MASK_BITS( UART4 )
#define PERI_CLK_CTRL115_REG_MASK() PERI_CLK_MASK_BITS( TWAI0, TWAI1, TWAI2 )
#define PERI_CLK_CTRL116_REG_MASK() PERI_CLK_MASK_BITS( GPSPI2_HS, GPSPI2_MST, GPSPI3_HS )
#define PERI_CLK_CTRL117_REG_MASK() PERI_CLK_MASK_BITS( GPSPI3_MST, PARLIO_RX )
#define PERI_CLK_CTRL118_REG_MASK() PERI_CLK_MASK_BITS( PARLIO_TX )
#define PERI_CLK_CTRL119_REG_MASK() PERI_CLK_MASK_BITS( I3C_MST, CAM )
#define PERI_CLK_CTRL20_REG_MASK()  PERI_CLK_MASK_BITS( MCPWM0, MCPWM1, TIMERGRP0_T0, TIMERGRP0_T1, TIMERGRP0_WDT, TIMERGRP0_TGRT )
#define PERI_CLK_CTRL21_REG_MASK()  PERI_CLK_MASK_BITS( TIMERGRP1_T0, TIMERGRP1_T1, TIMERGRP1_WDT, SYSTIMER )
#define PERI_CLK_CTRL22_REG_MASK()  PERI_CLK_MASK_BITS( LEDC, RMT )
#define PERI_CLK_CTRL23_REG_MASK()  PERI_CLK_MASK_BITS( ADC )
#define PERI_CLK_CTRL24_REG_MASK()  PERI_CLK_MASK_BITS( PVT )
#define PERI_CLK_CTRL25_REG_MASK0   PERI_CLK_MASK_BITS( PVT_PERI_GROUP1, PVT_PERI_GROUP2, PVT_PERI_GROUP3, PVT_PERI_GROUP4, CRYPTO_AES, CRYPTO_DS )
#define PERI_CLK_CTRL25_REG_MASK1   PERI_CLK_MASK_BITS( CRYPTO_ECC, CRYPTO_HMAC, CRYPTO_RSA, CRYPTO_SEC, CRYPTO_SHA, CRYPTO_ECDSA, CRYPTO_KM, ISP )
#define PERI_CLK_CTRL25_REG_MASK()  PERI_CLK_CTRL25_REG_MASK0 | PERI_CLK_CTRL25_REG_MASK1
#define PERI_CLK_CTRL26_REG_MASK()  PERI_CLK_MASK_BITS( IOMUX, H264, PADBIST_RX )
#define PERI_CLK_CTRL27_REG_MASK()  PERI_CLK_MASK_BITS( PADBIST_TX )


const static sleep_retention_entries_config_t clock_icg_config[] = {
    [0]  = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_CLOCK_ICG_LINK(0),  HP_SYS_CLKRST_PERI_CLK_CTRL00_REG,  0x0, PERI_CLK_CTRL00_REG_MASK(),  0, 1), .owner = ENTRY(0) },
    [1]  = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_CLOCK_ICG_LINK(1),  HP_SYS_CLKRST_PERI_CLK_CTRL01_REG,  0x0, PERI_CLK_CTRL01_REG_MASK(),  0, 1), .owner = ENTRY(0) },
    [2]  = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_CLOCK_ICG_LINK(2),  HP_SYS_CLKRST_PERI_CLK_CTRL02_REG,  0x0, PERI_CLK_CTRL02_REG_MASK(),  0, 1), .owner = ENTRY(0) },
    [3]  = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_CLOCK_ICG_LINK(3),  HP_SYS_CLKRST_PERI_CLK_CTRL03_REG,  0x0, PERI_CLK_CTRL03_REG_MASK(),  0, 1), .owner = ENTRY(0) },
    [4]  = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_CLOCK_ICG_LINK(4),  HP_SYS_CLKRST_PERI_CLK_CTRL10_REG,  0x0, PERI_CLK_CTRL10_REG_MASK(),  0, 1), .owner = ENTRY(0) },
    [5]  = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_CLOCK_ICG_LINK(5),  HP_SYS_CLKRST_PERI_CLK_CTRL11_REG,  0x0, PERI_CLK_CTRL11_REG_MASK(),  0, 1), .owner = ENTRY(0) },
    [6]  = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_CLOCK_ICG_LINK(6),  HP_SYS_CLKRST_PERI_CLK_CTRL13_REG,  0x0, PERI_CLK_CTRL13_REG_MASK(),  0, 1), .owner = ENTRY(0) },
    [7]  = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_CLOCK_ICG_LINK(7),  HP_SYS_CLKRST_PERI_CLK_CTRL14_REG,  0x0, PERI_CLK_CTRL14_REG_MASK(),  0, 1), .owner = ENTRY(0) },
    [8]  = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_CLOCK_ICG_LINK(8),  HP_SYS_CLKRST_PERI_CLK_CTRL15_REG,  0x0, PERI_CLK_CTRL15_REG_MASK(),  0, 1), .owner = ENTRY(0) },
    [9]  = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_CLOCK_ICG_LINK(9),  HP_SYS_CLKRST_PERI_CLK_CTRL17_REG,  0x0, PERI_CLK_CTRL17_REG_MASK(),  0, 1), .owner = ENTRY(0) },
    [10] = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_CLOCK_ICG_LINK(10), HP_SYS_CLKRST_PERI_CLK_CTRL18_REG,  0x0, PERI_CLK_CTRL18_REG_MASK(),  0, 1), .owner = ENTRY(0) },
    [11] = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_CLOCK_ICG_LINK(11), HP_SYS_CLKRST_PERI_CLK_CTRL19_REG,  0x0, PERI_CLK_CTRL19_REG_MASK(),  0, 1), .owner = ENTRY(0) },
    [12] = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_CLOCK_ICG_LINK(12), HP_SYS_CLKRST_PERI_CLK_CTRL110_REG, 0x0, PERI_CLK_CTRL110_REG_MASK(), 0, 1), .owner = ENTRY(0) },
    [13] = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_CLOCK_ICG_LINK(13), HP_SYS_CLKRST_PERI_CLK_CTRL111_REG, 0x0, PERI_CLK_CTRL111_REG_MASK(), 0, 1), .owner = ENTRY(0) },
    [14] = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_CLOCK_ICG_LINK(14), HP_SYS_CLKRST_PERI_CLK_CTRL112_REG, 0x0, PERI_CLK_CTRL112_REG_MASK(), 0, 1), .owner = ENTRY(0) },
    [15] = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_CLOCK_ICG_LINK(15), HP_SYS_CLKRST_PERI_CLK_CTRL113_REG, 0x0, PERI_CLK_CTRL113_REG_MASK(), 0, 1), .owner = ENTRY(0) },
    [16] = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_CLOCK_ICG_LINK(16), HP_SYS_CLKRST_PERI_CLK_CTRL114_REG, 0x0, PERI_CLK_CTRL114_REG_MASK(), 0, 1), .owner = ENTRY(0) },
    [17] = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_CLOCK_ICG_LINK(17), HP_SYS_CLKRST_PERI_CLK_CTRL115_REG, 0x0, PERI_CLK_CTRL115_REG_MASK(), 0, 1), .owner = ENTRY(0) },
    [18] = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_CLOCK_ICG_LINK(18), HP_SYS_CLKRST_PERI_CLK_CTRL116_REG, 0x0, PERI_CLK_CTRL116_REG_MASK(), 0, 1), .owner = ENTRY(0) },
    [19] = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_CLOCK_ICG_LINK(19), HP_SYS_CLKRST_PERI_CLK_CTRL117_REG, 0x0, PERI_CLK_CTRL117_REG_MASK(), 0, 1), .owner = ENTRY(0) },
    [20] = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_CLOCK_ICG_LINK(20), HP_SYS_CLKRST_PERI_CLK_CTRL118_REG, 0x0, PERI_CLK_CTRL118_REG_MASK(), 0, 1), .owner = ENTRY(0) },
    [21] = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_CLOCK_ICG_LINK(21), HP_SYS_CLKRST_PERI_CLK_CTRL119_REG, 0x0, PERI_CLK_CTRL119_REG_MASK(), 0, 1), .owner = ENTRY(0) },
    [22] = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_CLOCK_ICG_LINK(22), HP_SYS_CLKRST_PERI_CLK_CTRL20_REG,  0x0, PERI_CLK_CTRL20_REG_MASK(),  0, 1), .owner = ENTRY(0) },
    [23] = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_CLOCK_ICG_LINK(23), HP_SYS_CLKRST_PERI_CLK_CTRL21_REG,  0x0, PERI_CLK_CTRL21_REG_MASK(),  0, 1), .owner = ENTRY(0) },
    [24] = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_CLOCK_ICG_LINK(24), HP_SYS_CLKRST_PERI_CLK_CTRL22_REG,  0x0, PERI_CLK_CTRL22_REG_MASK(),  0, 1), .owner = ENTRY(0) },
    [25] = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_CLOCK_ICG_LINK(25), HP_SYS_CLKRST_PERI_CLK_CTRL23_REG,  0x0, PERI_CLK_CTRL23_REG_MASK(),  0, 1), .owner = ENTRY(0) },
    [26] = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_CLOCK_ICG_LINK(26), HP_SYS_CLKRST_PERI_CLK_CTRL24_REG,  0x0, PERI_CLK_CTRL24_REG_MASK(),  0, 1), .owner = ENTRY(0) },
    [27] = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_CLOCK_ICG_LINK(27), HP_SYS_CLKRST_PERI_CLK_CTRL25_REG,  0x0, PERI_CLK_CTRL25_REG_MASK(),  0, 1), .owner = ENTRY(0) },
    [28] = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_CLOCK_ICG_LINK(28), HP_SYS_CLKRST_PERI_CLK_CTRL26_REG,  0x0, PERI_CLK_CTRL26_REG_MASK(),  0, 1), .owner = ENTRY(0) },
    [29] = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_CLOCK_ICG_LINK(29), HP_SYS_CLKRST_PERI_CLK_CTRL27_REG,  0x0, PERI_CLK_CTRL27_REG_MASK(),  0, 1), .owner = ENTRY(0) },
    [30] = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_CLOCK_ICG_LINK(30), HP_SYS_CLKRST_SOC_CLK_CTRL2_REG,    0x0, SOC_CLK_CTRL2_REG_MASK(),    0, 1), .owner = ENTRY(0) },
    [31] = { .config = REGDMA_LINK_WRITE_INIT (REGDMA_CLOCK_ICG_LINK(31), HP_SYS_CLKRST_SOC_CLK_CTRL3_REG,    0x0, SOC_CLK_CTRL3_REG_MASK(),    0, 1), .owner = ENTRY(0) }
};

typedef struct {
    void *regdma_desc[ARRAY_SIZE(clock_icg_config)];
} pmu_sleep_clock_icg_context_t;

typedef struct {
    const sleep_retention_entries_config_t *config;
    int size;
} sleep_clock_icg_init_args_t;

static esp_err_t sleep_clock_icg_init(void *arg)
{
    sleep_clock_icg_init_args_t *init_args = (sleep_clock_icg_init_args_t *)arg;
    esp_err_t err = sleep_retention_entries_create(init_args->config, init_args->size, REGDMA_LINK_PRI_CLOCK_ICG, SLEEP_RETENTION_MODULE_CLOCK_ICG);
    if (err == ESP_OK) {
        sleep_retention_entries_config_t pcr_regs_retention[] = {
#define N_REGS_PCR()    (((HP_SYS_CLKRST_HPCORE_WDT_RESET_SOURCE0_REG - DR_REG_HP_SYS_CLKRST_BASE) / 4) + 1)
            { .config = REGDMA_LINK_CONTINUOUS_INIT(REGDMA_CLOCK_ICG_LINK(0xff), DR_REG_HP_SYS_CLKRST_BASE, DR_REG_HP_SYS_CLKRST_BASE, N_REGS_PCR(), 0, 0), .owner = ENTRY(0) }
#undef N_REGS_PCR
        };
        err = sleep_retention_entries_create(pcr_regs_retention, ARRAY_SIZE(pcr_regs_retention), REGDMA_LINK_PRI_CLOCK_ICG, SLEEP_RETENTION_MODULE_CLOCK_ICG);
    }
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for system sleep clock ICG, 1 level priority");
    ESP_LOGI(TAG, "System sleep clock ICG setting initialization");
    return ESP_OK;
}

ESP_SYSTEM_INIT_FN(sleep_clock_icg_startup_init, SECONDARY, BIT(0), 106)
{
    sleep_clock_icg_init_args_t init_args = { .config = clock_icg_config, .size = ARRAY_SIZE(clock_icg_config) };
    sleep_retention_module_init_param_t clock_icg_init_param = {
        .cbs = { .create = { .handle = sleep_clock_icg_init, .arg = (void *)&init_args } }
    };
    esp_err_t err = sleep_retention_module_init(SLEEP_RETENTION_MODULE_CLOCK_ICG, &clock_icg_init_param);
    if (err == ESP_OK) {
        err = sleep_retention_module_allocate(SLEEP_RETENTION_MODULE_CLOCK_ICG);
    }
    ESP_RETURN_ON_ERROR(err, TAG, "failed to allocate memory for system sleep clock ICG");

    static DRAM_ATTR pmu_sleep_clock_icg_context_t clock_icg_context;
    for (int i = 0; (err == ESP_OK) && (i < ARRAY_SIZE(clock_icg_config)); i++) {
        void *desc = sleep_retention_find_link_by_id(REGDMA_CLOCK_ICG_LINK(i));
        if (desc) {
            clock_icg_context.regdma_desc[i] = desc;
        } else {
            err = ESP_ERR_NOT_FOUND;
        }
    }
    if (err == ESP_OK) {
        PMU_instance()->priv = (void *)&clock_icg_context;
    }
    ESP_RETURN_ON_ERROR(err, TAG, "failed to initialize system sleep clock ICG context");
    return ESP_OK;
}


void pmu_sleep_clock_icg_config(void *icg_context, const uint32_t icg_func)
{
    assert(icg_context);
    pmu_sleep_clock_icg_context_t *clock_icg = (pmu_sleep_clock_icg_context_t *)icg_context;
    /* Locking is not required here because this function is always invoked
     * within esp_light_sleep_start(), where the necessary lock has already been acquired. */
    if (icg_func & BIT(PMU_ICG_FUNC_ENA_IOMUX)) {
        regdma_link_set_write_wait_content(clock_icg->regdma_desc[28], PERI_CLK_EN(IOMUX), PERI_CLK_MASK(IOMUX));
    }
    if (icg_func & BIT(PMU_ICG_FUNC_ENA_LEDC)) {
        regdma_link_set_write_wait_content(clock_icg->regdma_desc[24], PERI_CLK_EN(LEDC), PERI_CLK_MASK(LEDC));
    }
    if (icg_func & BIT(PMU_ICG_FUNC_ENA_UART0)) {
        regdma_link_set_write_wait_content(clock_icg->regdma_desc[12], PERI_CLK_EN(UART0), PERI_CLK_MASK(UART0));
    }
    if (icg_func & BIT(PMU_ICG_FUNC_ENA_UART1)) {
        regdma_link_set_write_wait_content(clock_icg->regdma_desc[13], PERI_CLK_EN(UART1), PERI_CLK_MASK(UART1));
    }
#if SOC_UART_HP_NUM > 2
    if (icg_func & BIT(PMU_ICG_FUNC_ENA_UART2)) {
        regdma_link_set_write_wait_content(clock_icg->regdma_desc[14], PERI_CLK_EN(UART2), PERI_CLK_MASK(UART2));
    }
#endif
#if SOC_UART_HP_NUM > 3
    if (icg_func & BIT(PMU_ICG_FUNC_ENA_UART3)) {
        regdma_link_set_write_wait_content(clock_icg->regdma_desc[15], PERI_CLK_EN(UART3), PERI_CLK_MASK(UART3));
    }
#endif
#if SOC_UART_HP_NUM > 4
    if (icg_func & BIT(PMU_ICG_FUNC_ENA_UART4)) {
        regdma_link_set_write_wait_content(clock_icg->regdma_desc[16], PERI_CLK_EN(UART4), PERI_CLK_MASK(UART4));
    }
#endif
}
