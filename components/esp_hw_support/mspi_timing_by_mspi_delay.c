/*
 * SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @brief
 *
 * This file contains configuration APIs doing MSPI timing tuning by MSPI delay
 * This file will only be built, when `SOC_MEMSPI_TIMING_TUNING_BY_MSPI_DELAY == 1`
 */

#include <sys/param.h>
#include "sdkconfig.h"
#include "string.h"
#include "esp_attr.h"
#include "esp_err.h"
#include "esp_types.h"
#include "esp_log.h"
#include "soc/rtc.h"
#include "hal/mspi_timing_tuning_ll.h"
#include "hal/clk_tree_ll.h"
#include "hal/regi2c_ctrl_ll.h"
#include "mspi_timing_config.h"
#include "mspi_timing_by_mspi_delay.h"
#include "bootloader_flash.h"
#include "esp32s3/rom/spi_flash.h"
#include "esp32s3/rom/opi_flash.h"

#define OPI_PSRAM_SYNC_READ           0x0000
#define OPI_PSRAM_SYNC_WRITE          0x8080
#define OCT_PSRAM_RD_DUMMY_NUM        (2*(10-1))
#define OCT_PSRAM_WR_DUMMY_NUM        (2*(5-1))

#define QPI_PSRAM_FAST_READ            0XEB
#define QPI_PSRAM_WRITE                0X38
#define QPI_PSRAM_FAST_READ_DUMMY      6

#define NOT_INIT_INT                  127

/////////////////////////////////////////TIMING TUNING IS NEEDED//////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#if MSPI_TIMING_FLASH_NEEDS_TUNING || MSPI_TIMING_PSRAM_NEEDS_TUNING
const static char *TAG = "MSPI Timing";

//If one of the FLASH / PSRAM or both of them need timing tuning, we should build following code
typedef enum {
    PSRAM_CMD_QPI,
    PSRAM_CMD_SPI,
} psram_cmd_mode_t;

static uint8_t s_rom_flash_extra_dummy[2] = {NOT_INIT_INT, NOT_INIT_INT};

#if CONFIG_SPIRAM_MODE_QUAD
static uint8_t s_psram_extra_dummy;
extern void psram_exec_cmd(int spi_num, psram_cmd_mode_t mode,
                           uint32_t cmd, int cmd_bit_len,
                           uint32_t addr, int addr_bit_len,
                           int dummy_bits,
                           uint8_t* mosi_data, int mosi_bit_len,
                           uint8_t* miso_data, int miso_bit_len,
                           uint32_t cs_mask,
                           bool is_write_erase_operation);
#endif

//-------------------------------------FLASH timing tuning register config-------------------------------------//
void mspi_timing_get_flash_tuning_configs(mspi_timing_config_t *config)
{
#if MSPI_TIMING_FLASH_DTR_MODE
#define FLASH_MODE  DTR_MODE
#else //MSPI_TIMING_FLASH_STR_MODE
#define FLASH_MODE  STR_MODE
#endif

#if CONFIG_ESPTOOLPY_FLASHFREQ_80M
    *config = MSPI_TIMING_FLASH_GET_TUNING_CONFIG(MSPI_TIMING_CORE_CLOCK_MHZ, 80, FLASH_MODE);
#elif CONFIG_ESPTOOLPY_FLASHFREQ_120M
    *config = MSPI_TIMING_FLASH_GET_TUNING_CONFIG(MSPI_TIMING_CORE_CLOCK_MHZ, 120, FLASH_MODE);
#else
    assert(false && "should never reach here");
#endif

#undef FLASH_MODE
}

void mspi_timing_flash_init(uint32_t flash_freq_mhz)
{
    mspi_timing_config_set_flash_clock(flash_freq_mhz, MSPI_TIMING_SPEED_MODE_NORMAL_PERF, true);

    //Power on HCLK
    mspi_timinng_ll_enable_flash_hclk(0);
}

static void s_set_flash_din_mode_num(uint8_t spi_num, uint8_t din_mode, uint8_t din_num)
{
    mspi_timing_ll_set_flash_din_mode(spi_num, din_mode);
    mspi_timing_ll_set_flash_din_num(spi_num, din_num);
}

static uint32_t spi_timing_config_get_dummy(void)
{
    mspi_timing_ll_flash_mode_t mode = mspi_timing_ll_get_flash_mode(0);
    if (mode == MSPI_TIMING_LL_FLASH_OPI_MODE) {
        abort();
    }

#if CONFIG_SPI_FLASH_HPM_DC_ON
    if (spi_flash_hpm_dummy_adjust()) { // HPM-DC is enabled
        const spi_flash_hpm_dummy_conf_t *hpm_dummy = spi_flash_hpm_get_dummy();
        switch (mode) {
            case MSPI_TIMING_LL_FLASH_QIO_MODE:
                return hpm_dummy->qio_dummy - 1;
            case MSPI_TIMING_LL_FLASH_QUAD_MODE:
                return hpm_dummy->qout_dummy - 1;
            case MSPI_TIMING_LL_FLASH_DIO_MODE:
                return hpm_dummy->dio_dummy - 1;
            case MSPI_TIMING_LL_FLASH_DUAL_MODE:
                return hpm_dummy->dout_dummy - 1;
            case MSPI_TIMING_LL_FLASH_FAST_MODE:
                return hpm_dummy->fastrd_dummy - 1;
            case MSPI_TIMING_LL_FLASH_SLOW_MODE:
                return 0;
            default:
                abort();
        }
    } else
#endif
    { // HPM-DC is not enabled
        switch (mode) {
            case MSPI_TIMING_LL_FLASH_QIO_MODE:
                return SPI1_R_QIO_DUMMY_CYCLELEN;
            case MSPI_TIMING_LL_FLASH_QUAD_MODE:
                return SPI1_R_FAST_DUMMY_CYCLELEN;
            case MSPI_TIMING_LL_FLASH_DIO_MODE:
                return SPI1_R_DIO_DUMMY_CYCLELEN;
            case MSPI_TIMING_LL_FLASH_DUAL_MODE:
                return SPI1_R_FAST_DUMMY_CYCLELEN;
            case MSPI_TIMING_LL_FLASH_FAST_MODE:
                return SPI1_R_FAST_DUMMY_CYCLELEN;
            case MSPI_TIMING_LL_FLASH_SLOW_MODE:
                return 0;
            default:
                abort();
        }
    }
}

static void s_set_flash_extra_dummy(uint8_t spi_num, uint8_t extra_dummy)
{
    if (bootloader_flash_is_octal_mode_enabled()) {
        mspi_timing_ll_set_octal_flash_extra_dummy(spi_num, extra_dummy);
        return;
    }
    /**
     * HW workaround:
     * The `SPI_MEM_TIMING_CALI_REG` register is only used for OPI on 728
     * Here we only need to update this global variable for extra dummy. Since we use the ROM Flash API, which will set the dummy based on this.
     * We only initialise the SPI0. And leave the SPI1 for flash driver to configure.
     */
    if (s_rom_flash_extra_dummy[spi_num] == NOT_INIT_INT) {
        s_rom_flash_extra_dummy[spi_num] = g_rom_spiflash_dummy_len_plus[spi_num];
    }
    g_rom_spiflash_dummy_len_plus[spi_num] = s_rom_flash_extra_dummy[spi_num] + extra_dummy;

    // Only Quad Flash will run into this branch.
    uint32_t dummy = spi_timing_config_get_dummy();
    mspi_timing_ll_set_quad_flash_dummy(spi_num, dummy + g_rom_spiflash_dummy_len_plus[spi_num]);
}

void mspi_timing_config_flash_set_tuning_regs(const void *timing_params)
{
    const mspi_timing_tuning_param_t *params = (const mspi_timing_tuning_param_t *)timing_params;
    /**
     * 1. SPI_MEM_DINx_MODE(1), SPI_MEM_DINx_NUM(1) are meaningless
     *    SPI0 and SPI1 share the SPI_MEM_DINx_MODE(0), SPI_MEM_DINx_NUM(0) for FLASH timing tuning
     * 2. We use SPI1 to get the best Flash timing tuning (mode and num) config
     */
    s_set_flash_din_mode_num(0, params->spi_din_mode, params->spi_din_num);
    s_set_flash_extra_dummy(1, params->extra_dummy_len);
}

//-------------------------------------------FLASH Read/Write------------------------------------------//
void mspi_timing_config_flash_read_data(uint8_t *buf, uint32_t addr, uint32_t len)
{
    if (bootloader_flash_is_octal_mode_enabled()) {
        // note that in spi_flash_read API, there is a wait-idle stage, since flash can only be read in idle state.
        // but after we change the timing settings, we might not read correct idle status via RDSR.
        // so, here we should use a read API that won't check idle status.
        mspi_timing_ll_clear_fifo(1);
        esp_rom_opiflash_read_raw(addr, buf, len);
    } else {
        esp_rom_spiflash_read(addr, (uint32_t *)buf, len);
    }
}

//-------------------------------------PSRAM timing tuning register config-------------------------------------//
void mspi_timing_get_psram_tuning_configs(mspi_timing_config_t *config)
{
#if MSPI_TIMING_PSRAM_DTR_MODE
#define PSRAM_MODE  DTR_MODE
#else //MSPI_TIMING_PSRAM_STR_MODE
#define PSRAM_MODE  STR_MODE
#endif

#if CONFIG_SPIRAM_SPEED_80M
    *config = MSPI_TIMING_PSRAM_GET_TUNING_CONFIG(MSPI_TIMING_CORE_CLOCK_MHZ, 80, PSRAM_MODE);
#elif CONFIG_SPIRAM_SPEED_120M
    *config = MSPI_TIMING_PSRAM_GET_TUNING_CONFIG(MSPI_TIMING_CORE_CLOCK_MHZ, 120, PSRAM_MODE);
#else
    assert(false && "should never reach here");
#endif

#undef PSRAM_MODE
}

void mspi_timing_psram_init(uint32_t psram_freq_mhz)
{
    mspi_timing_config_set_flash_clock(psram_freq_mhz, MSPI_TIMING_SPEED_MODE_NORMAL_PERF, true);

    //Power on HCLK
    mspi_timinng_ll_enable_psram_hclk(0);
}

static void s_set_psram_din_mode_num(uint8_t spi_num, uint8_t din_mode, uint8_t din_num)
{
    mspi_timing_ll_set_psram_din_mode(spi_num, din_mode);
    mspi_timing_ll_set_psram_din_num(spi_num, din_num);
}

static void s_set_psram_extra_dummy(uint8_t spi_num, uint8_t extra_dummy)
{
#if CONFIG_SPIRAM_MODE_OCT
    mspi_timing_ll_set_octal_psram_extra_dummy(spi_num, extra_dummy);
#elif CONFIG_SPIRAM_MODE_QUAD
    //HW workaround: Use normal dummy register to set extra dummy, the calibration dedicated extra dummy register doesn't work for quad mode
    mspi_timing_ll_set_quad_psram_dummy(spi_num, (QPI_PSRAM_FAST_READ_DUMMY + extra_dummy - 1));
#endif
}

void mspi_timing_config_psram_set_tuning_regs(const void *timing_params)
{
    const mspi_timing_tuning_param_t *params = (const mspi_timing_tuning_param_t *)timing_params;
    /**
     * 1. SPI_MEM_SPI_SMEM_DINx_MODE(1), SPI_MEM_SPI_SMEM_DINx_NUM(1) are meaningless
     *    SPI0 and SPI1 share the SPI_MEM_SPI_SMEM_DINx_MODE(0), SPI_MEM_SPI_SMEM_DINx_NUM(0) for PSRAM timing tuning
     * 2. We use SPI1 to get the best PSRAM timing tuning (mode and num) config
     */
    s_set_psram_din_mode_num(0, params->spi_din_mode, params->spi_din_num);

#if CONFIG_SPIRAM_MODE_OCT
    //On 728, for SPI1, flash and psram share the extra dummy register
    s_set_flash_extra_dummy(1, params->extra_dummy_len);
#elif CONFIG_SPIRAM_MODE_QUAD
    //Update this `s_psram_extra_dummy`, the `s_psram_read_data` will set dummy according to this `s_psram_extra_dummy`
    s_psram_extra_dummy = params->extra_dummy_len;
    mspi_timing_ll_set_quad_flash_dummy(1, params->extra_dummy_len - 1);
#endif
}

//-------------------------------------------PSRAM Read/Write------------------------------------------//
static void s_psram_write_data(uint8_t *buf, uint32_t addr, uint32_t len)
{
#if CONFIG_SPIRAM_MODE_OCT
    esp_rom_opiflash_exec_cmd(1, ESP_ROM_SPIFLASH_OPI_DTR_MODE,
                            OPI_PSRAM_SYNC_WRITE, 16,
                            addr, 32,
                            OCT_PSRAM_WR_DUMMY_NUM,
                            buf, len * 8,
                            NULL, 0,
                            BIT(1),
                            false);
#elif CONFIG_SPIRAM_MODE_QUAD
    psram_exec_cmd(1, 0,
                   QPI_PSRAM_WRITE, 8,
                   addr, 24,
                   0,
                   buf, len * 8,
                   NULL, 0,
                   SPI_MEM_CS1_DIS_M,
                   false);
#endif
}

static void s_psram_read_data(uint8_t *buf, uint32_t addr, uint32_t len)
{
#if CONFIG_SPIRAM_MODE_OCT
    mspi_timing_ll_clear_fifo(1);
    esp_rom_opiflash_exec_cmd(1, ESP_ROM_SPIFLASH_OPI_DTR_MODE,
                            OPI_PSRAM_SYNC_READ, 16,
                            addr, 32,
                            OCT_PSRAM_RD_DUMMY_NUM,
                            NULL, 0,
                            buf, len * 8,
                            BIT(1),
                            false);
#elif CONFIG_SPIRAM_MODE_QUAD
    psram_exec_cmd(1, 0,
                   QPI_PSRAM_FAST_READ, 8,
                   addr, 24,
                   QPI_PSRAM_FAST_READ_DUMMY + s_psram_extra_dummy,
                   NULL, 0,
                   buf, len * 8,
                   SPI_MEM_CS1_DIS_M,
                   false);
#endif
}

static void s_psram_execution(uint8_t *buf, uint32_t addr, uint32_t len, bool is_read)
{
    while (len) {
        uint32_t length = MIN(len, 32);
        if (is_read) {
            s_psram_read_data(buf, addr, length);
        } else {
            s_psram_write_data(buf, addr, length);
        }
        addr += length;
        buf += length;
        len -= length;
    }
}

void mspi_timing_config_psram_write_data(uint8_t *buf, uint32_t addr, uint32_t len)
{
    s_psram_execution(buf, addr, len, false);
}

void mspi_timing_config_psram_read_data(uint8_t *buf, uint32_t addr, uint32_t len)
{
    s_psram_execution(buf, addr, len, true);
}

/*-------------------------------------------------------------------------------------------------
 * Best Timing Tuning Params Selection
 *-------------------------------------------------------------------------------------------------*/
#if (MSPI_TIMING_FLASH_DTR_MODE || MSPI_TIMING_PSRAM_DTR_MODE) && (MSPI_TIMING_CORE_CLOCK_MHZ == 240)
static bool get_working_pll_freq(const uint8_t *reference_data, bool is_flash, uint32_t *out_max_freq, uint32_t *out_min_freq)
{
    uint8_t read_data[MSPI_TIMING_TEST_DATA_LEN] = {0};
    rtc_cpu_freq_config_t previous_config;
    rtc_clk_cpu_freq_get_config(&previous_config);

    uint32_t big_num = MSPI_TIMING_PLL_FREQ_SCAN_RANGE_MHZ_MAX * 2;  //This number should be larger than MSPI_TIMING_PLL_FREQ_SCAN_RANGE_MHZ_MAX, for error handling
    uint32_t max_freq = 0;
    uint32_t min_freq = big_num;
    rtc_xtal_freq_t xtal_freq = rtc_clk_xtal_freq_get();

    for (int pll_mhz_tuning = MSPI_TIMING_PLL_FREQ_SCAN_RANGE_MHZ_MIN; pll_mhz_tuning <= MSPI_TIMING_PLL_FREQ_SCAN_RANGE_MHZ_MAX; pll_mhz_tuning += 8) {
        //bbpll calibration start
        regi2c_ctrl_ll_bbpll_calibration_start();

        /**
         * pll_mhz = xtal_mhz * (oc_div + 4) / (oc_ref_div + 1)
         */
        clk_ll_bbpll_set_frequency_for_mspi_tuning(xtal_freq, pll_mhz_tuning, ((pll_mhz_tuning / 4) - 4), 9);

        //wait calibration done
        while(!regi2c_ctrl_ll_bbpll_calibration_is_done());

        //bbpll calibration stop
        regi2c_ctrl_ll_bbpll_calibration_stop();

        memset(read_data, 0, MSPI_TIMING_TEST_DATA_LEN);
        if (is_flash) {
            mspi_timing_config_flash_read_data(read_data, MSPI_TIMING_FLASH_TEST_DATA_ADDR, MSPI_TIMING_TEST_DATA_LEN);
        } else {
            mspi_timing_config_psram_read_data(read_data, MSPI_TIMING_PSRAM_TEST_DATA_ADDR, MSPI_TIMING_TEST_DATA_LEN);
        }

        if (memcmp(read_data, reference_data, MSPI_TIMING_TEST_DATA_LEN) == 0) {
            max_freq = MAX(pll_mhz_tuning, max_freq);
            min_freq = MIN(pll_mhz_tuning, min_freq);

            //Continue to find successful cases
            continue;
        }

        if (max_freq != 0) {
            //The first fail case after successful case(s) is the end
            break;
        }

        //If no break, no successful case found, continue to find successful cases
    }

    //restore PLL config
    clk_ll_bbpll_set_freq_mhz(previous_config.source_freq_mhz);
    //bbpll calibration start
    regi2c_ctrl_ll_bbpll_calibration_start();
    //set pll
    clk_ll_bbpll_set_config(previous_config.source_freq_mhz, xtal_freq);
    //wait calibration done
    while(!regi2c_ctrl_ll_bbpll_calibration_is_done());
    //bbpll calibration stop
    regi2c_ctrl_ll_bbpll_calibration_stop();

    *out_max_freq = max_freq;
    *out_min_freq = min_freq;

    return (max_freq != 0);
}
#endif  //Frequency Scanning

static uint32_t s_select_best_tuning_config_dtr(const mspi_timing_config_t *configs, uint32_t consecutive_length, uint32_t end, const uint8_t *reference_data, bool is_flash)
{
#if (MSPI_TIMING_CORE_CLOCK_MHZ == 160)
    //Core clock 160M DTR best point scheme
    uint32_t best_point;

    //Define these magic number in macros in `spi_timing_config.h`. TODO: IDF-3663
    if (consecutive_length <= 2 || consecutive_length >= 6) {
        //tuning is FAIL, select default point, and generate a warning
        best_point = configs->default_config_id;
        ESP_EARLY_LOGW(TAG, "tuning fail, best point is fallen back to index %"PRIu32"", best_point);
    } else if (consecutive_length <= 4) {
        //consecutive length :  3 or 4
        best_point = end - 1;
        ESP_EARLY_LOGD(TAG, "tuning success, best point is index %"PRIu32"", best_point);
    } else {
        //consecutive point list length equals 5
        best_point = end - 2;
        ESP_EARLY_LOGD(TAG, "tuning success, best point is index %"PRIu32"", best_point);
    }

    return best_point;

#elif (MSPI_TIMING_CORE_CLOCK_MHZ == 240)

    uint32_t best_point = 0;
    uint32_t current_point = end + 1 - consecutive_length;
    bool ret = false;

    //This `max_freq` is the max pll frequency that per MSPI timing tuning config can work
    uint32_t max_freq = 0;
    uint32_t temp_max_freq = 0;
    uint32_t temp_min_freq = 0;

    for (; current_point <= end; current_point++) {
        if (is_flash) {
            mspi_timing_config_flash_set_tuning_regs(&(configs->tuning_config_table[current_point]));
        } else {
            mspi_timing_config_psram_set_tuning_regs(&(configs->tuning_config_table[current_point]));
        }

        ret = get_working_pll_freq(reference_data, is_flash, &temp_max_freq, &temp_min_freq);
        if (ret && temp_min_freq <= MSPI_TIMING_PLL_FREQ_SCAN_THRESH_MHZ_LOW && temp_max_freq >= MSPI_TIMING_PLL_FREQ_SCAN_THRESH_MHZ_HIGH && temp_max_freq > max_freq) {
            max_freq = temp_max_freq;
            best_point = current_point;
        }
        ESP_EARLY_LOGD(TAG, "sample point %"PRIu32", max pll is %"PRIu32" mhz, min pll is %"PRIu32"\n", current_point, temp_max_freq, temp_min_freq);
    }
    if (max_freq == 0) {
        ESP_EARLY_LOGW(TAG, "freq scan tuning fail, best point is fallen back to index %"PRIu32"", end + 1 - consecutive_length);
        best_point = end + 1 - consecutive_length;
    } else {
        ESP_EARLY_LOGD(TAG, "freq scan success, max pll is %"PRIu32"mhz, best point is index %"PRIu32"", max_freq, best_point);
    }

    return best_point;

#else
    //won't reach here
    abort();
#endif
}

static uint32_t s_select_best_tuning_config_str(const mspi_timing_config_t *configs, uint32_t consecutive_length, uint32_t end)
{
#if (MSPI_TIMING_CORE_CLOCK_MHZ == 120 || MSPI_TIMING_CORE_CLOCK_MHZ == 240)
    //STR best point scheme
    uint32_t best_point;

    if (consecutive_length <= 2|| consecutive_length >= 5) {
        //tuning is FAIL, select default point, and generate a warning
        best_point = configs->default_config_id;
        ESP_EARLY_LOGW(TAG, "tuning fail, best point is fallen back to index %"PRIu32"", best_point);
    } else {
        //consecutive length :  3 or 4
        best_point = end - consecutive_length / 2;
        ESP_EARLY_LOGD(TAG, "tuning success, best point is index %"PRIu32"", best_point);
    }

    return best_point;
#else
    //won't reach here
    abort();
#endif
}

static uint32_t s_select_best_tuning_config(const mspi_timing_config_t *configs, uint32_t consecutive_length, uint32_t end, const uint8_t *reference_data, bool is_ddr, bool is_flash)
{
    uint32_t best_point = 0;
    if (is_ddr) {
        best_point = s_select_best_tuning_config_dtr(configs, consecutive_length, end, reference_data, is_flash);
    } else {
        best_point = s_select_best_tuning_config_str(configs, consecutive_length, end);
    }

    return best_point;
}

uint32_t mspi_timing_flash_select_best_tuning_config(const void *configs, uint32_t consecutive_length, uint32_t end, const uint8_t *reference_data, bool is_ddr)
{
    const mspi_timing_config_t *timing_configs = (const mspi_timing_config_t *)configs;
    uint32_t best_point = s_select_best_tuning_config(timing_configs, consecutive_length, end, reference_data, is_ddr, true);
    ESP_EARLY_LOGI(TAG, "Flash timing tuning index: %"PRIu32"", best_point);

    return best_point;
}

uint32_t mspi_timing_psram_select_best_tuning_config(const void *configs, uint32_t consecutive_length, uint32_t end, const uint8_t *reference_data, bool is_ddr)
{
    const mspi_timing_config_t *timing_configs = (const mspi_timing_config_t *)configs;
    uint32_t best_point = s_select_best_tuning_config(timing_configs, consecutive_length, end, reference_data, is_ddr, false);
    ESP_EARLY_LOGI(TAG, "PSRAM timing tuning index: %"PRIu32"", best_point);

    return best_point;
}

static mspi_timing_tuning_param_t s_flash_best_timing_tuning_config;
static mspi_timing_tuning_param_t s_psram_best_timing_tuning_config;

void mspi_timing_flash_set_best_tuning_config(const void *timing_params)
{
    const mspi_timing_tuning_param_t *params = (const mspi_timing_tuning_param_t *)timing_params;
    s_flash_best_timing_tuning_config = *params;
}

void mspi_timing_psram_set_best_tuning_config(const void *timing_params)
{
    const mspi_timing_tuning_param_t *params = (const mspi_timing_tuning_param_t *)timing_params;
    s_psram_best_timing_tuning_config = *params;
}


/*-------------------------------------------------------------------------------------------------
 * Best Timing Tuning Params Clear / Set
 *-------------------------------------------------------------------------------------------------*/
void mspi_timing_flash_config_clear_tuning_regs(bool control_both_mspi)
{
    s_set_flash_din_mode_num(0, 0, 0);  //SPI0 and SPI1 share the registers for flash din mode and num setting, so we only set SPI0's reg
    s_set_flash_extra_dummy(0, 0);

    //Won't touch SPI1 registers if not control_both_mspi
    if (control_both_mspi) {
        s_set_flash_extra_dummy(1, 0);
    }
}

void mspi_timing_flash_config_set_tuning_regs(bool control_both_mspi)
{
    //SPI0 and SPI1 share the registers for flash din mode and num setting, so we only set SPI0's reg
    s_set_flash_din_mode_num(0, s_flash_best_timing_tuning_config.spi_din_mode, s_flash_best_timing_tuning_config.spi_din_num);
    s_set_flash_extra_dummy(0, s_flash_best_timing_tuning_config.extra_dummy_len);
    if (control_both_mspi) {
        s_set_flash_extra_dummy(1, s_flash_best_timing_tuning_config.extra_dummy_len);
    } else {
        //Won't touch SPI1 registers
    }
}

void mspi_timing_psram_config_clear_tuning_regs(bool control_both_mspi)
{
    (void)control_both_mspi;  //for compatibility
    s_set_psram_din_mode_num(0, 0, 0);
    s_set_psram_extra_dummy(0, 0);
}

void mspi_timing_psram_config_set_tuning_regs(bool control_both_mspi)
{
    (void)control_both_mspi;  //for compatibility
    s_set_psram_din_mode_num(0, s_psram_best_timing_tuning_config.spi_din_mode, s_psram_best_timing_tuning_config.spi_din_num);
    s_set_psram_extra_dummy(0, s_psram_best_timing_tuning_config.extra_dummy_len);
}
#endif //#if MSPI_TIMING_FLASH_NEEDS_TUNING || MSPI_TIMING_PSRAM_NEEDS_TUNING


/*-------------------------------------------------------------------------------------------------
 * To let upper lay (spi_flash_timing_tuning.c) to know the necessary timing registers
 *-------------------------------------------------------------------------------------------------*/
/**
 * Get the SPI1 Flash CS timing setting. The setup time and hold time are both realistic cycles.
 * @note On ESP32-S3, SPI0/1 share the Flash CS timing registers. Therefore, we should not change these values.
 * @note This function inform `spi_flash_timing_tuning.c` (driver layer) of the cycle,
 * and other component (esp_flash driver) should get these cycle and configure the registers accordingly.
 */
void mspi_timing_config_get_cs_timing(uint8_t *setup_time, uint32_t *hold_time)
{
    *setup_time = mspi_timing_ll_get_cs_setup_val(0);
    *hold_time = mspi_timing_ll_get_cs_hold_val(0);
    /**
     * The logic here is, if setup_en / hold_en is false, then we return the realistic cycle number,
     * which is 0. If true, then the realistic cycle number is (reg_value + 1)
     */
    if (mspi_timing_ll_is_cs_setup_enabled(0)) {
        *setup_time += 1;
    } else {
        *setup_time = 0;
    }
    if (mspi_timing_ll_is_cs_hold_enabled(0)) {
        *hold_time += 1;
    } else {
        *hold_time = 0;
    }
}

/**
 * Get the SPI1 Flash clock setting.
 * @note Similarly, this function inform `spi_flash_timing_tuning.c` (driver layer) of the clock setting,
 * and other component (esp_flash driver) should get these and configure the registers accordingly.
 */
uint32_t mspi_timing_config_get_flash_clock_reg(void)
{
    return mspi_timing_ll_get_clock_reg(1);
}

uint8_t mspi_timing_config_get_flash_extra_dummy(void)
{
#if MSPI_TIMING_FLASH_NEEDS_TUNING
    return s_flash_best_timing_tuning_config.extra_dummy_len;
#else
    return 0;
#endif
}
