/**
 * BMX280 - BME280和BMP280驱动程序，适用于Espressif ESP-32。
 *
 * MIT 许可协议
 *
 * 版权所有 (C) 2020 Halit Utku Maden
 * 请通过 <utkumaden@hotmail.com> 联系我
 */

// 法律声明：
// 以下代码段在“// HERE BE DRAGONS”标题下方和“// END OF DRAGONS”标题上方
// 包含由博世（Bosch）Sensortec GmbH拥有的代码的修改版本，
// 该代码未明确许可，因此本存储库的MIT许可不涵盖此代码。
// 使用者需自行承担风险。

#include "bmx280.h"
#include "esp_log.h"

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

// [BME280] 湿度最低有效字节的寄存器地址。
#define BMX280_REG_HUMI_LSB 0xFE
// [BME280] 湿度最高有效字节的寄存器地址。
#define BMX280_REG_HUMI_MSB 0xFD

// 温度分数字节的寄存器地址。
#define BMX280_REG_TEMP_XSB 0xFC
// 温度最低有效字节的寄存器地址。
#define BMX280_REG_TEMP_LSB 0xFB
// 温度最高有效字节的寄存器地址。
#define BMX280_REG_TEMP_MSB 0xFA

// 压力分数字节的寄存器地址。
#define BMX280_REG_PRES_XSB 0xF9
// 压力最低有效字节的寄存器地址。
// 注册地址的压力最低有效字节。
#define BMX280_REG_PRES_LSB 0xF8
// 注册地址的压力最高有效字节。
#define BMX280_REG_PRES_MSB 0xF7

// 注册地址的传感器配置。
#define BMX280_REG_CONFIG 0xF5
// 注册地址的传感器测量控制。
#define BMX280_REG_MESCTL 0xF4
// 注册地址的传感器状态。
#define BMX280_REG_STATUS 0xF3
// [BME280] 湿度控制的寄存器地址。
#define BMX280_REG_HUMCTL 0xF2

// [BME280] 校准常量的寄存器地址。（高库）
#define BMX280_REG_CAL_HI 0xE1
// 校准常量的寄存器地址。（低库）
#define BMX280_REG_CAL_LO 0x88

// 注册地址的传感器复位。
#define BMX280_REG_RESET 0xE0
// 芯片复位向量。
#define BMX280_RESET_VEC 0xB6

// 注册地址的芯片识别号。
#define BMX280_REG_CHPID 0xD0
// BME280的REG_CHPID值
#define BME280_ID 0x60
// BMP280（工程样品1）的REG_CHPID值
#define BMP280_ID0 0x56
// BMP280（工程样品2）的REG_CHPID值
#define BMP280_ID1 0x57
// BMP280（生产）的REG_CHPID值
#define BMP280_ID2 0x58

struct bmx280_t {
    // I2C端口。
    i2c_port_t i2c_port;
    // 传感器的从地址。
    uint8_t slave;
    // 传感器的芯片ID。
    uint8_t chip_id;
    // 补偿数据
    struct {
        uint16_t T1;
        int16_t T2;
        int16_t T3;
        uint16_t P1;
        int16_t P2;
        int16_t P3;
        int16_t P4;
        int16_t P5;
        int16_t P6;
        int16_t P7;
        int16_t P8;
        int16_t P9;
#if !(CONFIG_BMX280_EXPECT_BMP280)
        uint8_t H1;
        int16_t H2;
        uint8_t H3;
        int16_t H4;
        int16_t H5;
        int8_t H6;
#endif
    } cmps;
    // 存储与温度成比例的变量。
    int32_t t_fine;
};

/**
 * 宏定义：识别芯片ID是否为BME280或BMP280
 * @note 仅在验证芯片为BME280或BMP280时使用。
 * @see bmx280_verify
 * @param chip_id 芯片ID。
 */
#define bmx280_isBME(chip_id) ((chip_id) == BME280_ID)

/**
 * 宏定义：验证芯片ID与预期值是否匹配。
 * @note 当需要验证芯片为BME280或BMP280时使用。
 * @see bmx280_isBME
 * @param chip_id 芯片ID。
 */
#define bmx280_verify(chip_id)                                                                                         \
    (((chip_id) == BME280_ID) || ((chip_id) == BMP280_ID2) || ((chip_id) == BMP280_ID1) || ((chip_id) == BMP280_ID0))

/**
 * 如果未找到传感器，则返回false。
 * @param bmx280 驱动程序结构。
 */
#define bmx280_validate(bmx280) (!(bmx280->slave == 0xDE && bmx280->chip_id == 0xAD))

/**
 * 从传感器读取数据。
 * @param bmx280 驱动程序结构。
 * @param addr 寄存器地址。
 * @param dout 读取的数据。
 * @param size 读取的字节数。
 * @returns 错误代码。
 */
static esp_err_t bmx280_read(bmx280_t *bmx280, uint8_t addr, uint8_t *dout, size_t size) {
    esp_err_t err;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    if (cmd) {
        // 写入寄存器地址
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, bmx280->slave | I2C_MASTER_WRITE, true);
        i2c_master_write_byte(cmd, addr, true);

        // 读取寄存器
        i2c_master_start(cmd);
        i2c_master_write_byte(cmd, bmx280->slave | I2C_MASTER_READ, true);
        i2c_master_read(cmd, dout, size, I2C_MASTER_LAST_NACK);
        i2c_master_stop(cmd);

        err = i2c_master_cmd_begin(bmx280->i2c_port, cmd, CONFIG_BMX280_TIMEOUT);
        i2c_cmd_link_delete(cmd);
        return err;
    } else {
        return ESP_ERR_NO_MEM;
    }
}

static esp_err_t bmx280_write(bmx280_t *bmx280, uint8_t addr, const uint8_t *din, size_t size) {
    esp_err_t err;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    if (cmd) {
        for (int i = 0; i < size; i++) {
            i2c_master_start(cmd);
            i2c_master_write_byte(cmd, bmx280->slave | I2C_MASTER_WRITE, true);
            // 寄存器
            i2c_master_write_byte(cmd, addr + i, true);
            // 数据
            i2c_master_write_byte(cmd, din[i], true);
        }
        i2c_master_stop(cmd);

        err = i2c_master_cmd_begin(bmx280->i2c_port, cmd, CONFIG_BMX280_TIMEOUT);
        i2c_cmd_link_delete(cmd);
        return err;
    } else {
        return ESP_ERR_NO_MEM;
    }
}

static esp_err_t bmx280_probe_address(bmx280_t *bmx280) {
    esp_err_t err = bmx280_read(bmx280, BMX280_REG_CHPID, &bmx280->chip_id, sizeof bmx280->chip_id);

    if (err == ESP_OK) {
        if (
#if CONFIG_BMX280_EXPECT_BME280
            bmx280->chip_id == BME280_ID
#elif CONFIG_BMX280_EXPECT_BMP280
            bmx280->chip_id == BMP280_ID2 || bmx280->chip_id == BMP280_ID1 || bmx280->chip_id == BMP280_ID0
#else
            bmx280_verify(bmx280->chip_id)
#endif
        ) {
            ESP_LOGI("bmx280", "探测成功: 地址=%hhx, ID=%hhx", bmx280->slave, bmx280->chip_id);
            return ESP_OK;
        } else {
            err = ESP_ERR_NOT_FOUND;
        }
    }

    ESP_LOGW("bmx280", "探测失败: 地址=%hhx, ID=%hhx, 原因=%s", bmx280->slave, bmx280->chip_id, esp_err_to_name(err));
    return err;
}

static esp_err_t bmx280_probe(bmx280_t *bmx280) {
    ESP_LOGI("bmx280", "在I2C %d上探测BMP280/BME280传感器", bmx280->i2c_port);

#if CONFIG_BMX280_ADDRESS_HI
    bmx280->slave = 0xEE;
    return bmx280_probe_address(bmx280);
#elif CONFIG_BMX280_ADDRESS_LO
    bmx280->slave = 0xEC;
    return bmx280_probe_address(bmx280);
#else
    esp_err_t err;
    bmx280->slave = 0xEC;
    if ((err = bmx280_probe_address(bmx280)) != ESP_OK) {
        bmx280->slave = 0xEE;
        if ((err = bmx280_probe_address(bmx280)) != ESP_OK) {
            ESP_LOGE("bmx280", "未找到传感器。");
            bmx280->slave = 0xDE;
            bmx280->chip_id = 0xAD;
        }
    }
    return err;
#endif
}

static esp_err_t bmx280_reset(bmx280_t *bmx280) {
    const static uint8_t din[] = {BMX280_RESET_VEC};
    return bmx280_write(bmx280, BMX280_REG_RESET, din, sizeof din);
}

static esp_err_t bmx280_calibrate(bmx280_t *bmx280) {
    // 诚实地说，最好的做法是将高库和低库的内容读取到一个缓冲区，
    // 然后将其放入校准值中，使代码端序无关，
    // 并克服结构对齐问题。
    // 此外，BME280的高库是奇怪的。

    ESP_LOGI("bmx280", "读取校准值...");

    esp_err_t err;
    uint8_t buf[26];

    // 低库
    err = bmx280_read(bmx280, BMX280_REG_CAL_LO, buf, sizeof buf);

    if (err != ESP_OK)
        return err;

    ESP_LOGI("bmx280", "读取低库成功。");

    bmx280->cmps.T1 = buf[0] | (buf[1] << 8);
    bmx280->cmps.T2 = buf[2] | (buf[3] << 8);
    bmx280->cmps.T3 = buf[4] | (buf[5] << 8);
    bmx280->cmps.P1 = buf[6] | (buf[7] << 8);
    bmx280->cmps.P2 = buf[8] | (buf[9] << 8);
    bmx280->cmps.P3 = buf[10] | (buf[11] << 8);
    bmx280->cmps.P4 = buf[12] | (buf[13] << 8);
    bmx280->cmps.P5 = buf[14] | (buf[15] << 8);
    bmx280->cmps.P6 = buf[16] | (buf[17] << 8);
    bmx280->cmps.P7 = buf[18] | (buf[19] << 8);
    bmx280->cmps.P8 = buf[20] | (buf[21] << 8);
    bmx280->cmps.P9 = buf[22] | (buf[23] << 8);

#if !(CONFIG_BMX280_EXPECT_BMP280)

#if CONFIG_BMX280_EXPECT_DETECT
    if (bmx280_isBME(bmx280->chip_id)) // 仅在检测场景中使用此条件。
#endif
    {
        // 首先获取H1。
        bmx280->cmps.H1 = buf[23];

        err = bmx280_read(bmx280, BMX280_REG_CAL_HI, buf, 7);

        if (err != ESP_OK)
            return err;

        ESP_LOGI("bmx280", "读取高库成功。");

        bmx280->cmps.H2 = buf[0] | (buf[1] << 8);
        bmx280->cmps.H3 = buf[2];
        bmx280->cmps.H4 = (buf[3] << 4) | (buf[4] & 0x0F);
        bmx280->cmps.H5 = (buf[4] >> 4) | (buf[5] << 4);
        bmx280->cmps.H6 = buf[6];
    }

#endif

    return ESP_OK;
}

bmx280_t *bmx280_create(i2c_port_t port) {
    bmx280_t *bmx280 = malloc(sizeof(bmx280_t));
    if (bmx280) {
        memset(bmx280, 0, sizeof(bmx280_t));

        bmx280->i2c_port = port;
        bmx280->slave = 0xDE;
        bmx280->chip_id = 0xAD;
    }
    return bmx280;
}

void bmx280_close(bmx280_t *bmx280) {
    free(bmx280);
}

esp_err_t bmx280_init(bmx280_t *bmx280) {
    if (bmx280 == NULL)
        return ESP_ERR_INVALID_ARG;

    esp_err_t error = bmx280_probe(bmx280) || bmx280_reset(bmx280);

    if (error == ESP_OK) {
        // 给传感器10ms的延迟以进行复位。
        vTaskDelay(pdMS_TO_TICKS(10));

        // 读取校准数据。
        bmx280_calibrate(bmx280);

        ESP_LOGI("bmx280", "转储校准数据...");
        ESP_LOG_BUFFER_HEX("bmx280", &bmx280->cmps, sizeof(bmx280->cmps));
    }

    return error;
}

esp_err_t bmx280_configure(bmx280_t *bmx280, bmx280_config_t *cfg) {
    if (bmx280 == NULL || cfg == NULL)
        return ESP_ERR_INVALID_ARG;
    if (!bmx280_validate(bmx280))
        return ESP_ERR_INVALID_STATE;

    // 始终先设置ctrl_meas。
    uint8_t num = (cfg->t_sampling << 5) | (cfg->p_sampling << 2) | BMX280_MODE_SLEEP;
    esp_err_t err = bmx280_write(bmx280, BMX280_REG_MESCTL, &num, sizeof num);

    if (err)
        return err;

    // 现在可以设置配置。
    num = (cfg->t_standby << 5) | (cfg->iir_filter << 2);
    err = bmx280_write(bmx280, BMX280_REG_CONFIG, &num, sizeof num);

    if (err)
        return err;

#if !(CONFIG_BMX280_EXPECT_BMP280)
#if CONFIG_BMX280_EXPECT_DETECT
    if (bmx280_isBME(bmx280->chip_id))
#elif CONFIG_BMX280_EXPECT_BME280
#endif
    {
        num = cfg->h_sampling;
        err = bmx280_write(bmx280, BMX280_REG_HUMCTL, &num, sizeof(num));

        if (err)
            return err;
    }
#endif

    // f = 0;
    return ESP_OK;
}

esp_err_t bmx280_setMode(bmx280_t *bmx280, bmx280_mode_t mode) {
    uint8_t ctrl_mes;
    esp_err_t err;

    if ((err = bmx280_read(bmx280, BMX280_REG_MESCTL, &ctrl_mes, 1)) != ESP_OK)
        return err;

    ctrl_mes = (ctrl_mes & (~3)) | mode;

    return bmx280_write(bmx280, BMX280_REG_MESCTL, &ctrl_mes, 1);
}

esp_err_t bmx280_getMode(bmx280_t *bmx280, bmx280_mode_t *mode) {
    uint8_t ctrl_mes;
    esp_err_t err;

    if ((err = bmx280_read(bmx280, BMX280_REG_MESCTL, &ctrl_mes, 1)) != ESP_OK)
        return err;

    ctrl_mes &= 3;

    switch (ctrl_mes) {
    default:
        *mode = ctrl_mes;
        break;
    case (BMX280_MODE_FORCE + 1):
        *mode = BMX280_MODE_FORCE;
        break;
    }

    return ESP_OK;
}

bool bmx280_isSampling(bmx280_t *bmx280) {
    uint8_t status;
    if (bmx280_read(bmx280, BMX280_REG_STATUS, &status, 1) == ESP_OK)
        return (status & (1 << 3)) != 0;
    else
        return false;
}
// 法律声明：
// 以下代码段在“// HERE BE DRAGONS”标题下方和“// END OF DRAGONS”标题上方
// 包含由博世（Bosch）Sensortec GmbH拥有的代码的修改版本，
// 该代码未明确许可，因此本存储库的MIT许可不涵盖此代码。
// 使用者需自行承担风险。

// HERE BE DRAGONS
// 这些代码是从BME280的数据手册中借鉴的博世代码。
// 我对它的工作原理了解不多，所以无法告诉你它的具体功能。
// 请勿修改。

// 返回温度（单位：摄氏度），分辨率为0.01摄氏度。
// 输出值“5123”代表51.23摄氏度。
// t_fine为全局变量，存储精确温度
int32_t BME280_compensate_T_int32(bmx280_t *bmx280, int32_t adc_T) {
    int32_t var1, var2, T;
    var1 = ((((adc_T >> 3) - ((int32_t)bmx280->cmps.T1 << 1))) * ((int32_t)bmx280->cmps.T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)bmx280->cmps.T1)) * ((adc_T >> 4) - ((int32_t)bmx280->cmps.T1))) >> 12) *
            ((int32_t)bmx280->cmps.T3)) >>
           14;
    bmx280->t_fine = var1 + var2;
    T = (bmx280->t_fine * 5 + 128) >> 8;
    return T;
}

// 返回压力（单位：帕斯卡），以无符号32位整数的Q24.8格式表示（24位整数和8位小数）。
// 输出值“24674867”代表24674867/256 = 96386.2 Pa = 963.862 hPa
uint32_t BME280_compensate_P_int64(bmx280_t *bmx280, int32_t adc_P) {
    int64_t var1, var2, p;
    var1 = ((int64_t)bmx280->t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)bmx280->cmps.P6;
    var2 = var2 + ((var1 * (int64_t)bmx280->cmps.P5) << 17);
    var2 = var2 + (((int64_t)bmx280->cmps.P4) << 35);
    var1 = ((var1 * var1 * (int64_t)bmx280->cmps.P3) >> 8) + ((var1 * (int64_t)bmx280->cmps.P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1) * ((int64_t)bmx280->cmps.P1)) >> 33;
    if (var1 == 0) {
        return 0; // 避免因除以零而引发异常
    }
    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)bmx280->cmps.P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)bmx280->cmps.P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)bmx280->cmps.P7) << 4);
    return (uint32_t)p;
}

#if !CONFIG_BMX280_EXPECT_BMP280

// 返回湿度（单位：%RH），以无符号32位整数的Q22.10格式表示（22位整数和10位小数）。
// 输出值“47445”代表47445/1024 = 46.333 %RH
uint32_t bme280_compensate_H_int32(bmx280_t *bmx280, int32_t adc_H) {
    int32_t v_x1_u32r;
    v_x1_u32r = (bmx280->t_fine - ((int32_t)76800));
    v_x1_u32r = (((((adc_H << 14) - (((int32_t)bmx280->cmps.H4) << 20) - (((int32_t)bmx280->cmps.H5) * v_x1_u32r)) +
                   ((int32_t)16384)) >>
                  15) *
                 (((((((v_x1_u32r * ((int32_t)bmx280->cmps.H6)) >> 10) *
                      (((v_x1_u32r * ((int32_t)bmx280->cmps.H3)) >> 11) + ((int32_t)32768))) >>
                     10) +
                    ((int32_t)2097152)) *
                       ((int32_t)bmx280->cmps.H2) +
                   8192) >>
                  14));
    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t)bmx280->cmps.H1)) >> 4));
    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
    return (uint32_t)(v_x1_u32r >> 12);
}

#endif

// END OF DRAGONS

esp_err_t bmx280_readout(bmx280_t *bmx280, int32_t *temperature, uint32_t *pressure, uint32_t *humidity) {
    if (bmx280 == NULL)
        return ESP_ERR_INVALID_ARG;
    if (!bmx280_validate(bmx280))
        return ESP_ERR_INVALID_STATE;

    uint8_t buffer[3];
    esp_err_t error;

    if (temperature) {
        if ((error = bmx280_read(bmx280, BMX280_REG_TEMP_MSB, buffer, 3)) != ESP_OK)
            return error;

        *temperature = BME280_compensate_T_int32(bmx280, (buffer[0] << 12) | (buffer[1] << 4) | (buffer[0] >> 4));
    }

    if (pressure) {
        if ((error = bmx280_read(bmx280, BMX280_REG_PRES_MSB, buffer, 3)) != ESP_OK)
            return error;

        *pressure = BME280_compensate_P_int64(bmx280, (buffer[0] << 12) | (buffer[1] << 4) | (buffer[0] >> 4));
    }

#if !(CONFIG_BMX280_EXPECT_BMP280)
#if CONFIG_BMX280_EXPECT_DETECT
    if (bmx280_isBME(bmx280->chip_id))
#elif CONFIG_BMX280_EXPECT_BME280
#endif
    {
        if (humidity) {
            if ((error = bmx280_read(bmx280, BMX280_REG_HUMI_MSB, buffer, 2)) != ESP_OK)
                return error;

            *humidity = bme280_compensate_H_int32(bmx280, (buffer[0] << 8) | buffer[1]);
        }
    }
#if CONFIG_BMX280_EXPECT_DETECT
    else if (humidity)
        *humidity = UINT32_MAX;
#endif
#else
    if (humidity)
        *humidity = UINT32_MAX;
#endif

    return ESP_OK;
}
