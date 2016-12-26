#include "i2c_soft.h"
#include "rom/ets_sys.h"
#include "freertos/xtensa_api.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdio.h>
#include "ES8388.h"

#define ES8388_ADDR 0x22  // 0x22:CE=1;0x20:CE=0

static bool I2C_Write_Byte(uint8_t data, uint8_t iter);
void ES8388_WriteReg(uint8_t slave_add, uint8_t reg_add, uint8_t data);
uint8_t ES8388_ReadReg(uint8_t slave_add, uint8_t reg_add, uint8_t dat);


void ES8388_Init(uint32_t mode, uint32_t channel, uint32_t bits, uint32_t rate)
{

}

void ES8388_Start()
{

}

void ES8388_Stop()
{

}

void ES8388_Pause()
{

}

void ES8388_Resume()
{

}

void ES8388_SetVolume()
{

}

void ES8388_SetDacChannel(int32_t channel, int32_t value)
{

}

void ES8388_SetMute(int32_t channel)
{

}

void ES8388_SetMicGain()
{

}


void ES8388_PowerOn_Init(void)
{
    ES8388_WriteReg(ES8388_ADDR, 0x08, 0x00); //CODEC IN I2S SLAVE MODE -

    ES8388_WriteReg(ES8388_ADDR, 0x00, 0x07);   //PLAY BACK,ENREFR=0 //changed
    ES8388_WriteReg(ES8388_ADDR, 0x2d, 0x00);   //vroi=0

    vTaskDelay(1);

    ES8388_WriteReg(ES8388_ADDR, 0x01, 0x40);   //pdn_ana=1,ibiasgen_pdn=0,lpvrefbuf=1

    vTaskDelay(1);

    ES8388_WriteReg(ES8388_ADDR, 0x02, 0xf3);   //Reset STM and DLL
    ES8388_WriteReg(ES8388_ADDR, 0x03, 0xFF);   //Reset STM and DLL
    ES8388_WriteReg(ES8388_ADDR, 0x04, 0xC0);   //power up L/R DAC,Enable LO1/RO1,LO2/RO2,OUT3/MONO

    ES8388_WriteReg(ES8388_ADDR, 0x0A, 0x00);  //LIN1/RIN1 as ADC Input
    ES8388_WriteReg(ES8388_ADDR, 0x0C, 0x0C);  //I2S-16BIT,LADC=Left data,RADC=right data W
    ES8388_WriteReg(ES8388_ADDR, 0x0d, 0x02);  //SINGLE SPEED,RATIO=256
    ES8388_WriteReg(ES8388_ADDR, 0x10, 0x00);  //ADC Left Volume=0db
    ES8388_WriteReg(ES8388_ADDR, 0x11, 0x00);  //ADC Right Volume=0db

    ES8388_WriteReg(ES8388_ADDR, 0x17, 0x18);  //I2S-16BIT
    ES8388_WriteReg(ES8388_ADDR, 0x18, 0x02);  //SINGLE SPEED,RATIO=256
    ES8388_WriteReg(ES8388_ADDR, 0x19, 0x36);  //SOFT RAMP RATE=32LRCKS/STEP,Enable ZERO-CROSS CHECK,DAC MUTE
    ES8388_WriteReg(ES8388_ADDR, 0x1A, 0x00);  //LDAC volume=0db,RDAC Volume=0db
    ES8388_WriteReg(ES8388_ADDR, 0x1B, 0x00);
    ES8388_WriteReg(ES8388_ADDR, 0x19, 0x32);  //SOFT RAMP RATE=32LRCKS/STEP,Enable ZERO-CROSS CHECK,DAC MUTE
    ES8388_WriteReg(ES8388_ADDR, 0x27, 0xB8);  //Left DAC TO Left MIXER
    ES8388_WriteReg(ES8388_ADDR, 0x28, 0x38);
    ES8388_WriteReg(ES8388_ADDR, 0x29, 0x38);
    ES8388_WriteReg(ES8388_ADDR, 0x2A, 0xB8);  //RIGHT DAC TO RIGHT MIXER
    ES8388_WriteReg(ES8388_ADDR, 0x04, 0x30);  //Enable DAC and Enable Lout/Rout
    ES8388_WriteReg(ES8388_ADDR, 0x2e, 0X10);  //LOUT/ROUT volume
    ES8388_WriteReg(ES8388_ADDR, 0x2f, 0X10);

    vTaskDelay(5);

    ES8388_WriteReg(ES8388_ADDR, 0x2e, 0x1A);  //LOUT/ROUT volume
    ES8388_WriteReg(ES8388_ADDR, 0x2f, 0x1A);

    ES8388_WriteReg(ES8388_ADDR, 0x04, 0xc0);  //Disable DAC and  Lout/Rout
}



void ES8388_DAC_Play()
{
    ES8388_WriteReg(ES8388_ADDR, 0x02, 0x00);  //START DAC DLL and State Machine
    ES8388_WriteReg(ES8388_ADDR, 0x2d, 0X00);  //vroi=0   DelayT0(10);          //Delay 10mS
    ES8388_WriteReg(ES8388_ADDR, 0x04, 0x3C);  //Enable DAC and Enable Lout/Rout

    vTaskDelay(1);

    ES8388_WriteReg(ES8388_ADDR, 0x2e, Vol);  //LOUT2/ROUT2
    ES8388_WriteReg(ES8388_ADDR, 0x2f, Vol);
    ES8388_WriteReg(ES8388_ADDR, 0x30, Vol);  //LOUT1/ROUT1
    ES8388_WriteReg(ES8388_ADDR, 0x31, Vol);

    ES8388_WriteReg(ES8388_ADDR, 0x19, 0xF2);   //DAC un-Mute
    ES8388_WriteReg(ES8388_ADDR, 0x01, 0x40);  //LPVrefBuf=0,Pdn_ana=0
    ES8388_WriteReg(ES8388_ADDR, 0x00, 0x07);  //Enfr=0,Play&Record Mode,(0x17-both of mic&paly)
}
void ES8388_ALC()
{

    ES8388_WriteReg(ES8388_ADDR, 0x12, 0xf2);  //ALC for Microphone
    ES8388_WriteReg(ES8388_ADDR, 0x13, 0xb0);
    ES8388_WriteReg(ES8388_ADDR, 0x14, 0x05);
    ES8388_WriteReg(ES8388_ADDR, 0x15, 0x06);
    ES8388_WriteReg(ES8388_ADDR, 0x16, 0x7b);

}
void ES8388_ADC_Record()
{

    /*
        ES8388_WriteReg(ES8388_ADDR, 0x08, 0x00); //Codec in I2S slave mode
        ES8388_WriteReg(ES8388_ADDR, 0x02, 0xF3); //ADC Power-up

        ES8388_WriteReg(ES8388_ADDR, 0x00, 0x05); //DAC Power-DN,LOUT/ROUT disable
        ES8388_WriteReg(ES8388_ADDR, 0x01, 0x40); //DAC Power-DN,LOUT/ROUT disable

        ES8388_WriteReg(ES8388_ADDR, 0x03, 0x00); //ADC Power-up
        ES8388_WriteReg(ES8388_ADDR, 0x04, 0xC0); //DAC Power-DN,LOUT/ROUT disable

        ES8388_WriteReg(ES8388_ADDR, 0x09, 0x77); //Analog In PGA=24db

        ES8388_WriteReg(ES8388_ADDR, 0x0A, 0x00); //MIC INPUT FROM LIN1/RIN1
        // ES8388_WriteReg(0x0A,0x50); //MIC INPUT FROM LIN2/RIN2

        ES8388_WriteReg(ES8388_ADDR, 0x0C, 0x0C); //I2S-16BIT, leftADC-Left Channel, RightADC-Right Channel
        ES8388_WriteReg(ES8388_ADDR, 0x0D, 0x02); //MCLK/LRCK=256

        ES8388_WriteReg(ES8388_ADDR, 0x10, 0x00); //ADC VOLUME = 0dB
        ES8388_WriteReg(ES8388_ADDR, 0x10, 0x00); //ADC VOLUME = 0dB

        ES8388_WriteReg(ES8388_ADDR, 0x02, 0x55);

        vTaskDelay(1);          //Delay 10mS
        ES8388_WriteReg(ES8388_ADDR, 0x01, 0x40); //pdn_ana=0,ibiasgen_pdn=0
        ES8388_WriteReg(ES8388_ADDR, 0x00, 0x05); //置ES8350于PLAYBACK & RECORD 模式，且EnRefr=1
                 ES8388_ALC();
                 ES8388_ADC_StandBy();
    */
    //unsigned int i;
    ES8388_WriteReg(ES8388_ADDR, 0x01, 0x36); //Power down whole chip analog
    ES8388_WriteReg(ES8388_ADDR, 0x01, 0x72); //power up whole chip analog
    //ES8388_WriteReg(ES8388_ADDR, 0x02, 0xF3); //stop STM and DLL, power down DAC&ADC vref
    //ES8388_WriteReg(ES8388_ADDR, 0x02, 0xF0); //power up DAC&ADC vref
    ES8388_WriteReg(ES8388_ADDR, 0x2B, 0x80); //set internal ADC and DAC use the same LRCK clock, ADC LRCK as internal LRCK
    ES8388_WriteReg(ES8388_ADDR, 0x02, 0xF3); //ADC clock is same as DAC. Use ADC MCLK as internal MCLK
    ES8388_WriteReg(ES8388_ADDR, 0x08, 0x00); //CODEC IN I2S SLAVE MODE-0x00;master-0x80
    ES8388_WriteReg(ES8388_ADDR, 0x04, 0xC0); //power down DAC, Disable LOUT&ROUT
    ES8388_WriteReg(ES8388_ADDR, 0x05, 0x00); //low power setting
    ES8388_WriteReg(ES8388_ADDR, 0x06, 0xC3);
    ES8388_WriteReg(ES8388_ADDR, 0x0A, 0x00); //select LIN1&RIN1 as Microphone
    ES8388_WriteReg(ES8388_ADDR, 0x0B, 0x00);
//  ES8388_WriteReg(ES8388_ADDR, 0x0C, 0x6F); //ADC 16BIT PCM & right=left

    ES8388_WriteReg(ES8388_ADDR, 0x0C, 0x0C); //ADC I2S-16BIT
    ES8388_WriteReg(ES8388_ADDR, 0x0D, 0x02); //ADCLRCK = MCLK/256
    ES8388_WriteReg(ES8388_ADDR, 0x10, 0x00); //ADC VOLUME = 0DB
    ES8388_WriteReg(ES8388_ADDR, 0x11, 0x00);
    ES8388_WriteReg(ES8388_ADDR, 0x09, 0x88); //MIC PGA =24DB
    ES8388_WriteReg(ES8388_ADDR, 0x12, 0xE2); //MIC ALC SETTING
    ES8388_WriteReg(ES8388_ADDR, 0x13, 0xC0);
    ES8388_WriteReg(ES8388_ADDR, 0x14, 0x12);
    ES8388_WriteReg(ES8388_ADDR, 0x15, 0x06);
    ES8388_WriteReg(ES8388_ADDR, 0x16, 0xC3);
    ES8388_WriteReg(ES8388_ADDR, 0x02, 0x00); //startup FSM and DLL
    ES8388_WriteReg(ES8388_ADDR, 0x03, 0x00); //Power up ADC, Enable LIN&RIN, Power down MICBIAS, set int1lp to low power mode
    vTaskDelay(1);
    //ES8388_WriteReg(ES8388_ADDR, 0x01, 0x40); //pdn_ana=0,ibiasgen_pdn=0
    //ES8388_WriteReg(ES8388_ADDR, 0x00, 0x05); //置ES8350于PLAYBACK & RECORD 模式，且EnRefr=1

}
void ES8388_ADC_StandBy()
{


    ES8388_WriteReg(ES8388_ADDR, 0x00, 0x01); //置ES8350于PLAYBACK & RECORD 模式，且EnRefr=1
    ES8388_WriteReg(ES8388_ADDR, 0x01, 0x58); //pdn_ana=0,ibiasgen_pdn=0
    vTaskDelay(1);        //Delay 10mS
    ES8388_WriteReg(ES8388_ADDR, 0x03, 0xFF); //ADC Power-Down
    ES8388_WriteReg(ES8388_ADDR, 0x04, 0xC0); //DAC Power-DN,LOUT/ROUT disable
    ES8388_WriteReg(ES8388_ADDR, 0x02, 0xf3);  //关闭录音

}
void ES8388_init(void)
{
    i2c_master_gpio_init();
    ES8388_PowerOn_Init();
// ES8388_ADC_Record();
    //ES8388_DAC_Play();

}

static bool I2C_Write_Byte(uint8_t data, uint8_t iter)
{
    if (iter == 0) {
        iter = 1;
    }

    while (iter--) {
        i2c_master_writeByte(data);

        if (i2c_master_getAck()) {
            i2c_master_stop();
            ets_delay_us(500);  // Wait 500us and retry.
            i2c_master_start();
        } else {
            return true;
        }
    }
    i2c_master_stop();
    return false;
}

void ES8388_WriteReg(uint8_t slave_add, uint8_t reg_add, uint8_t data)
{
    i2c_master_start();
    if (false == I2C_Write_Byte(slave_add, 30)) {
        printf("Slave is busy, TIME OUT");
    }
    if (false == I2C_Write_Byte(reg_add, 30)) {
        printf("Slave is busy, TIME OUT");
    }
    if (false == I2C_Write_Byte(data, 30)) {
        printf("Slave is busy, TIME OUT");
    }
    i2c_master_stop();
}

uint8_t ES8388_ReadReg(uint8_t slave_add, uint8_t reg_add, uint8_t dat)
{
    i2c_master_start();
    if (false == I2C_Write_Byte(slave_add, 10)) {
        printf("Slave is busy, TIME OUT");
    }
    if (false == I2C_Write_Byte(reg_add, 10)) {
        printf("Slave is busy, TIME OUT");
    }
    i2c_master_start();
    i2c_master_writeByte(0x21);
    uint8_t ack = i2c_master_getAck();
    uint8_t Reve = i2c_master_readByte();
    i2c_master_setAck(1);
    i2c_master_stop();
    return Reve;
}
