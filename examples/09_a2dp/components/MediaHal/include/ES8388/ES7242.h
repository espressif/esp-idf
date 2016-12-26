#ifndef __ES8388_H
#define __ES8388_H

#include "esp_types.h"
#include "rom/ets_sys.h"

#define ES7154_Address 0x00
#define ES7242_Address1 0x20
#define ES7242_Address2 0x22
#define ES7242_Address3 0x24
#define ES7242_Address4 0x26

void ES7242_init(void);
void ES7242_Poweron_Init(void);
//void ES8388_DAC_Play(void);
//void ES8388_ALC(void);
void ES7242_ADC_Record(void);
void ES7242_ADC_StandBy(void);
static bool I2C_Write_Byte(uint8_t data, uint8_t iter);
void ES7242_WriteReg(uint8_t slave_add, uint8_t reg_add, uint8_t data);
uint8_t ES7242_ReadReg(uint8_t slave_add, uint8_t reg_add, uint8_t dat);
void I2C_MasterInit(uint8_t init_num);
void i2c_master_read_test();


#endif

