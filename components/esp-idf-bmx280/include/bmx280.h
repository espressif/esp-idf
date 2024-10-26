/**
 * BMX280 - BME280与BMP280的驱动程序，适用于Espressif ESP-32。
 *
 * MIT许可证
 *
 * 版权所有 (C) 2020 Halit Utku Maden
 * 联系邮箱：<utkumaden@hotmail.com>
 */

#ifndef _BMX280_H_
#define _BMX280_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "driver/i2c.h"
#include "sdkconfig.h"
#include <limits.h>
#include <stdint.h>

#define BMXAPI extern

/**
 * 用于驱动设置的匿名结构体。
 */
typedef struct bmx280_t bmx280_t;

#include "bmx280_bits.h"

/**
 * 创建一个BMX280驱动程序的实例。
 * @param port 要使用的I2C端口。
 * @return 成功时返回指向驱动结构的非空指针。
 */
BMXAPI bmx280_t *bmx280_create(i2c_port_t port);
/**
 * 销毁实例。
 * @param bmx280 要销毁的实例。
 */
BMXAPI void bmx280_close(bmx280_t *bmx280);

/**
 * 侦测传感器并读取校准数据。
 * @param bmx280 驱动结构。
 */
BMXAPI esp_err_t bmx280_init(bmx280_t *bmx280);
/**
 * 使用给定参数配置传感器。
 * @param bmx280 驱动结构。
 * @param configuration 要使用的配置。
 */
BMXAPI esp_err_t bmx280_configure(bmx280_t *bmx280, bmx280_config_t *cfg);

/**
 * 设置传感器的工作模式。
 * @param bmx280 驱动结构。
 * @param mode 要设置的传感器模式。
 */
BMXAPI esp_err_t bmx280_setMode(bmx280_t *bmx280, bmx280_mode_t mode);
/**
 * 获取传感器当前的工作模式。
 * @param bmx280 驱动结构。
 * @param mode 指针，用于写入当前模式。
 */
BMXAPI esp_err_t bmx280_getMode(bmx280_t *bmx280, bmx280_mode_t *mode);

/**
 * 如果传感器当前正在采样环境条件，则返回真。
 * @param bmx280 驱动结构。
 */
BMXAPI bool bmx280_isSampling(bmx280_t *bmx280);

/**
 * 读取传感器值作为定点数。
 * @param bmx280 驱动结构。
 * @param temperature 摄氏度中的温度 (0.01度C递增)
 * @param pressure 压力 (以帕斯卡为单位，1/256帕斯卡递增)
 * @param humidity 湿度 (%RH，1/1024 %RH递增)；无效时为UINT32_MAX。
 */
BMXAPI esp_err_t bmx280_readout(bmx280_t *bmx280, int32_t *temperature, uint32_t *pressure, uint32_t *humidity);

/**
 * 将传感器读取值转换为浮点值。
 * @param tin 输入温度。
 * @param pin 输入压力。
 * @param hin 输入湿度。
 * @param tout 输出温度（摄氏度）。
 * @param pout 输出压力（帕斯卡）。
 * @param hout 输出湿度（%Rh）。
 */
static inline void bmx280_readout2float(int32_t *tin, uint32_t *pin, uint32_t *hin, float *tout, float *pout,
                                        float *hout) {
    if (tin && tout)
        *tout = (float)*tin * 0.01f; // 将摄氏度转换为浮点数
    if (pin && pout)
        *pout = (float)*pin * (1.0f / 256.0f); // 将压力转换为浮点数
    if (hin && hout)
        *hout = (*hin == UINT32_MAX) ? -1.0f : (float)*hin * (1.0f / 1024.0f); // 将湿度转换为浮点数
}

/**
 * 读取传感器值作为浮点数。
 * @param bmx280 驱动结构。
 * @param temperature 摄氏度中的温度。
 * @param pressure 压力（帕斯卡）。
 * @param humidity 湿度（%RH）。
 */
static inline esp_err_t bmx280_readoutFloat(bmx280_t *bmx280, float *temperature, float *pressure, float *humidity) {
    int32_t t;
    uint32_t p, h;
    esp_err_t err = bmx280_readout(bmx280, &t, &p, &h);

    if (err == ESP_OK) {
        bmx280_readout2float(&t, &p, &h, temperature, pressure, humidity);
    }

    return err;
}

#ifdef __cplusplus
};
#endif

#endif
