#include "driver/i2c.h"
#include "esp_types.h"
#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/xtensa_api.h"
#include "esp_err.h"
#include "driver/i2c_soft.h"
#include "ES7242.h"
#include "driver/gpio.h"
//#include "driver/gpio_sig_map.h"
#include "rom/ets_sys.h"



#define I2C_FREQ    100000   //HZ
#define I2C_SLAVE_DEV_ADDR 0x60

#define I2C_SDA_OUT_IO_NUM   21
#define I2C_SDA_OUT_IO_SIG   I2CEXT0_SDA_OUT_IDX
#define I2C_SDA_OUT_IO_PIN   GPIO_SEL_21

#define I2C_SCL_OUT_IO_NUM   19
#define I2C_SCL_OUT_IO_SIG   I2CEXT0_SCL_OUT_IDX
#define I2C_SCL_OUT_IO_PIN   GPIO_SEL_19

#define I2C_SDA_IN_IO_SIG    I2CEXT0_SDA_IN_IDX
#define I2C_SCL_IN_IO_SIG    I2CEXT0_SCL_IN_IDX

#define ES8388_ADDR 0x20

#if 0


void I2C_GpioInit()
{
#if 1
    ets_printf("Configuring GPIO...........\n");
    gpio_config_t gpio_conf;
    gpio_conf.gpio_pin_sel = I2C_SDA_OUT_IO_PIN | I2C_SCL_OUT_IO_PIN ;
    gpio_conf.gpio_mode_sel = GPIO_MODE_INPUT_OUTPUT_OD;
    gpio_conf.gpio_pulldown_sel = GPIO_PULLDOWN_DISABLE;
    gpio_conf.gpio_pullup_sel = GPIO_PULLUP_ENABLE;
    gpio_conf.gpio_intry_type_sel = GPIO_PIN_INTR_DISABLE;
    gpio_config(&gpio_conf);
#if 0
    SET_PERI_REG_BITS(GPIO_PIN_REG_19, FUN_DRV, 3, FUN_DRV_S);
    SET_PERI_REG_BITS(GPIO_PIN_REG_23, FUN_DRV, 3, FUN_DRV_S);
    CLEAR_PERI_REG_MASK(GPIO_PIN_REG_19, FUN_PD);
    CLEAR_PERI_REG_MASK(GPIO_PIN_REG_23, FUN_PD);
#endif

    gpio_matrix_out(I2C_SDA_OUT_IO_NUM, I2C_SDA_OUT_IO_SIG, 0, 0);
    gpio_matrix_out(I2C_SCL_OUT_IO_NUM, I2C_SCL_OUT_IO_SIG, 0, 0);
    gpio_matrix_in(I2C_SDA_OUT_IO_NUM, I2C_SDA_IN_IO_SIG, 0);
    gpio_matrix_in(I2C_SCL_OUT_IO_NUM, I2C_SCL_IN_IO_SIG, 0);
#else
    // SDA OUT
    gpio_matrix_out(19, I2C_SDA_OUT_IO_SIG, 0, 0);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO19_U, FUNC_GPIO19_GPIO19);
    SET_PERI_REG_MASK(PERIPHS_IO_MUX_GPIO19_U, FUN_IE | FUN_PU);
    CLEAR_PERI_REG_MASK(PERIPHS_IO_MUX_GPIO19_U, FUN_PD);

    SET_PERI_REG_MASK(GPIO_PIN19_REG, GPIO_PIN19_PAD_DRIVER);
    //====debug====
    gpio_matrix_out(23, I2C_SCL_OUT_IO_SIG, 0, 0);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO23_U, FUNC_GPIO23_GPIO23);
    SET_PERI_REG_MASK(PERIPHS_IO_MUX_GPIO23_U, FUN_IE | FUN_PU);
    CLEAR_PERI_REG_MASK(PERIPHS_IO_MUX_GPIO23_U, FUN_PD);
    SET_PERI_REG_MASK(GPIO_PIN23_REG, GPIO_PIN23_PAD_DRIVER);
    //============================
    //SDA IN
    gpio_matrix_in(19, I2C_SDA_IN_IO_SIG, 0);
    //SCL IN
    gpio_matrix_in(23, I2C_SCL_IN_IO_SIG, 0);

#endif

    ets_printf("GPIO Configured! ! ! ! ! ! ! ! ! !\n");
}


void disp_cmd(uint8_t i2c_no, uint8_t idx)
{
//  ets_printf("---cmd %d ---\n",idx);

//  ets_printf("opcode: %d \n",I2C_GET_COMMAND_OP_CODE(i2c_no,idx));
//  ets_printf("acken: %d \n",I2C_GET_COMMAND_ACK_EN(i2c_no,idx));
//  ets_printf("ackexp: %d \n",I2C_GET_COMMAND_ACK_EXP(i2c_no,idx));
//  ets_printf("ack val: %d \n",I2C_GET_COMMAND_ACK_VALUE(i2c_no,idx));
//  ets_printf("byte num: %d \n",I2C_GET_COMMAND_BYTE_NUM(i2c_no,idx));
//  ets_printf("done: %d \n",I2C_GET_COMMAND_DONE(i2c_no,idx));

}


void i2c_master_read_test()

{
    uint8_t i2c_no = 0;
    uint8_t idx = 0;

    uint8_t data_buf[] = {I2C_SLAVE_DEV_ADDR, //addr
                          0x10,
                          0x01,
                          0x12,
                          0x13,
                          0x14,
                         };
    uint8_t byte_num = 3;//sizeof(data_buf);

    I2C_SET_TX_FIFO_RST(0, 1);
    I2C_SET_TX_FIFO_RST(0, 0);

    //command0 : sending restart signal
    idx = 0;
    I2C_SET_COMMAND_OP_CODE(i2c_no, idx, 0);
    I2C_SET_COMMAND_ACK_EN(i2c_no, idx, 0);
    I2C_SET_COMMAND_BYTE_NUM(i2c_no, 0, 1);
    disp_cmd( i2c_no, 0);

    //command1 : picking slave and sending data
    idx = 1;
    I2C_SET_COMMAND_ACK_EN(i2c_no, idx, 0);
    I2C_SET_COMMAND_OP_CODE(i2c_no, idx, 1); //WRITE COMMAND
    I2C_SET_COMMAND_BYTE_NUM(i2c_no, idx, byte_num); //BYTE NUMBER
    disp_cmd( i2c_no, 1);

    //command2 : master generating stop
    idx = 2;
    I2C_SET_COMMAND_OP_CODE(i2c_no, idx, 0x3); //STOP
    I2C_SET_COMMAND_ACK_EN(i2c_no, idx, 0); //NO ACK
    disp_cmd( i2c_no, 2);

    int i = 0;
    for (i = 0; i < byte_num; i++) {
        WRITE_PERI_REG(I2C_DATA_REG(i2c_no), data_buf[i]);
    }

    //start sending
    I2C_SET_TRANS_START(i2c_no, 1);
}



void I2C_MasterInit(uint8_t init_num)
{

    //step 1: gpio init
    I2C_GpioInit();
#if 1
    ets_delay_us(500000);
    //step 2: i2c init
    ets_printf("Initializing I2C Master.............\r\n");
    I2C_InitTypeDef i2c_master;
    i2c_master.mode = I2C_MASTER_MODE;  //master mode
    i2c_master.addr_10bit_en = 0;       //not used
    i2c_master.clk_speed = 100000;      //clk set
    i2c_master.slave_addr = 0x0;        //no slave addr for master mode
    I2C_Init(init_num, &i2c_master);
#else
    uint8_t i2c_no = init_num;
    if (i2c_no > 1) {
        ets_printf("invalid i2c number...\r\n");
        return;
    }

    //set master mode
    //I2C_SET_RX_LSB_FIRST(i2c_no,0); //MSB FIRST
    //I2C_SET_TX_LSB_FIRST(i2c_no,0); //MSB FIRST
    I2C_SET_MS_MODE(i2c_no, 1); //MASTER MODE
    //I2C_SET_SCL_FORCE_OUT(i2c_no,1); //out put practical level
    //I2C_SET_SDA_FORCE_OUT(i2c_no,1); //out put practical level

    //SET FREQ/DUTY
    I2C_SET_SCL_LOW_PERIOD(i2c_no, (APB_CLK_FREQ / I2C_FREQ) / 2);
    I2C_SET_SCL_HIGH_PERIOD(i2c_no, (APB_CLK_FREQ / I2C_FREQ) / 2);

    //SET CLK RE/START HOLD/SETUP
    I2C_SET_SCL_START_HOLD_TIME(i2c_no, 50); //400);
    I2C_SET_SCL_RSTART_SETUP_TIME(i2c_no, 50); //400);

    //SET CLK STOP HOLD/SETUP
    I2C_SET_SCL_STOP_HOLD_TIME(i2c_no, 50); //400);
    I2C_SET_SCL_STOP_SETUP_TIME(i2c_no, 50); //400);

    //SET DATA I/O HOLD/SAMPLE
    I2C_SET_SDA_HOLD_TIME(i2c_no, 40);
    I2C_SET_SDA_SAMPLE_TIME(i2c_no, 40);

    //SET CLK TIME OUT
    //I2C_SET_TIME_OUT_THRSH(i2c_no,2000);
    I2C_SET_TIME_OUT_REG(i2c_no, 2000);

    //SET FIFO MODE
    //I2C_SET_NONFIFO_EN(i2c_no,0);
    //SET SCL FILTER
    //I2C_SET_SCL_FILTER_EN(i2c_no,1);
    //SET SDA FILTER
    //I2C_SET_SDA_FILTER_EN(i2c_no,0);

    //i2c_intr_config();

#endif

}





void i2c_send_data_test(uint8_t i2c_num, uint8_t slave_addr, uint8_t reg_addr, uint8_t data)
{


    //struct I2C_DEV * I2Cx = I2C(i2c_num);
    uint8_t data_buf[] = {
        slave_addr,
        reg_addr,
        data
    };

    uint8_t byte_num = sizeof(data_buf);
    struct I2C_CmdDef cmd;
    memset(&cmd, 0, sizeof(cmd));

    //reset tx fifo
    I2C_ResetTxFifo(i2c_num);

    //setup command 0
    memset(&cmd, 0, sizeof(cmd));
    cmd.ack_en = 0;
    cmd.ack_exp = 0;
    cmd.ack_val = 0;
    cmd.byte_num = 0;
    cmd.op_code = I2C_CMD_RESTART;

    I2C_ConfigCmd(i2c_num, 0, &cmd);

    disp_cmd( i2c_num, 0);

    //setup command 1
    memset(&cmd, 0, sizeof(cmd));
    cmd.ack_en = 0;
    cmd.ack_exp = 0;
    cmd.ack_val = 0;
    cmd.byte_num = byte_num;
    cmd.op_code = I2C_CMD_WRITE;
    I2C_ConfigCmd(i2c_num, 1, &cmd);
    disp_cmd( i2c_num, 1);

    //setup command 2
    memset(&cmd, 0, sizeof(cmd));
    cmd.byte_num = 0;
    cmd.op_code = I2C_CMD_STOP;
    I2C_ConfigCmd(i2c_num, 2, &cmd);
    disp_cmd( i2c_num, 2);

    //push the sending data to tx fifo
    I2C_TxFifoPush(i2c_num, data_buf , byte_num);

    //I2C start sending
    I2C_Start(i2c_num);

    I2C_WaitTxDone(i2c_num, 2);


}
#endif

void ES8388_PowerOn_Init1(void)
{
    ES7242_WriteReg(ES8388_ADDR, 0x08, 0x00); //CODEC IN I2S SLAVE MODE -

    ES7242_WriteReg(ES8388_ADDR, 0x00, 0x07);   //PLAY BACK,ENREFR=0 //changed
    ES7242_WriteReg(ES8388_ADDR, 0x2d, 0x00);   //vroi=0

    ets_delay_us(10000);

    ES7242_WriteReg(ES8388_ADDR, 0x01, 0x40);   //pdn_ana=1,ibiasgen_pdn=0,lpvrefbuf=1

    ets_delay_us(10000);

    ES7242_WriteReg(ES8388_ADDR, 0x02, 0xf3);   //Reset STM and DLL
    ES7242_WriteReg(ES8388_ADDR, 0x03, 0xFF);   //Reset STM and DLL
    ES7242_WriteReg(ES8388_ADDR, 0x04, 0xC0);   //power up L/R DAC,Enable LO1/RO1,LO2/RO2,OUT3/MONO

    ES7242_WriteReg(ES8388_ADDR, 0x0A, 0x00);  //LIN1/RIN1 as ADC Input
    ES7242_WriteReg(ES8388_ADDR, 0x0C, 0x0C);  //I2S-16BIT,LADC=Left data,RADC=right data W
    ES7242_WriteReg(ES8388_ADDR, 0x0d, 0x02);  //SINGLE SPEED,RATIO=256
    ES7242_WriteReg(ES8388_ADDR, 0x10, 0x00);  //ADC Left Volume=0db
    ES7242_WriteReg(ES8388_ADDR, 0x11, 0x00);  //ADC Right Volume=0db

    ES7242_WriteReg(ES8388_ADDR, 0x17, 0x18);  //I2S-16BIT 0x18 //PCM 16BIT 0x5E
    ES7242_WriteReg(ES8388_ADDR, 0x18, 0x02);  //SINGLE SPEED,RATIO=256
    ES7242_WriteReg(ES8388_ADDR, 0x19, 0x36);  //SOFT RAMP RATE=32LRCKS/STEP,Enable ZERO-CROSS CHECK,DAC MUTE
    ES7242_WriteReg(ES8388_ADDR, 0x1A, 0x00);  //LDAC volume=0db,RDAC Volume=0db
    ES7242_WriteReg(ES8388_ADDR, 0x1B, 0x00);
    ES7242_WriteReg(ES8388_ADDR, 0x19, 0x32);  //SOFT RAMP RATE=32LRCKS/STEP,Enable ZERO-CROSS CHECK,DAC MUTE
    ES7242_WriteReg(ES8388_ADDR, 0x27, 0xB8);  //Left DAC TO Left MIXER
    ES7242_WriteReg(ES8388_ADDR, 0x28, 0x38);
    ES7242_WriteReg(ES8388_ADDR, 0x29, 0x38);
    ES7242_WriteReg(ES8388_ADDR, 0x2A, 0xB8);  //RIGHT DAC TO RIGHT MIXER
    ES7242_WriteReg(ES8388_ADDR, 0x04, 0x30);  //Enable DAC and Enable Lout/Rout
    ES7242_WriteReg(ES8388_ADDR, 0x2e, 0X10);  //LOUT/ROUT volume
    ES7242_WriteReg(ES8388_ADDR, 0x2f, 0X10);

    ets_delay_us(50000);


    ES7242_WriteReg(ES8388_ADDR, 0x2e, 0x1A);  //LOUT/ROUT volume
    ES7242_WriteReg(ES8388_ADDR, 0x2f, 0x1A);

    ES7242_WriteReg(ES8388_ADDR, 0x04, 0xc0);  //Disable DAC and  Lout/Rout
}

void ES8388_DAC_Play1(void)
{
    int Vol = 0x1f;
    ES7242_WriteReg(ES8388_ADDR, 0x02, 0xAA);  //START DAC DLL and State Machine
    ES7242_WriteReg(ES8388_ADDR, 0x2d, 0X00);  //vroi=0   DelayT0(10);          //Delay 10mS
    ES7242_WriteReg(ES8388_ADDR, 0x04, 0x3C);  //Enable DAC and Enable Lout/Rout

    ets_delay_us(10000);


    ES7242_WriteReg(ES8388_ADDR, 0x2e, Vol);  //LOUT2/ROUT2
    ES7242_WriteReg(ES8388_ADDR, 0x2f, Vol);
    ES7242_WriteReg(ES8388_ADDR, 0x30, Vol);  //LOUT1/ROUT1
    ES7242_WriteReg(ES8388_ADDR, 0x31, Vol);

    ES7242_WriteReg(ES8388_ADDR, 0x19, 0xF2);   //DAC un-Mute
    ES7242_WriteReg(ES8388_ADDR, 0x01, 0x40);  //LPVrefBuf=0,Pdn_ana=0
    ES7242_WriteReg(ES8388_ADDR, 0x00, 0x07);  //Enfr=0,Play&Record Mode,(0x17-both of mic&paly)
}


void ES7242_Poweron_Init(void)
{
    ets_printf("Writing Data to Registers\n");
#if 0
    ES7242_WriteReg(ES7242_Address1, 0x00, 0x01);
    ets_delay_us(10000);
    ES7242_WriteReg(ES7242_Address1, 0x01, 0x00);
    ets_delay_us(10000);
    ES7242_WriteReg(ES7242_Address2, 0x00, 0x01);
    ets_delay_us(10000);
    ES7242_WriteReg(ES7242_Address2, 0x01, 0x00);
    ets_delay_us(10000);
#endif
#if 1
    ES7242_WriteReg(ES7242_Address3, 0x00, 0x01);
    ets_delay_us(10000);
    ES7242_WriteReg(ES7242_Address3, 0x01, 0x00);
    ets_delay_us(10000);
    ES7242_WriteReg(ES7242_Address4, 0x00, 0x01);
    ets_delay_us(10000);
    ES7242_WriteReg(ES7242_Address4, 0x01, 0x00);
    ets_delay_us(10000);
#endif
}

void ES7242_ADC_Record()
{

#if 0
    ES7242_WriteReg(ES7242_Address1, 0x01, 0x0F);
    ES7242_WriteReg(ES7242_Address1, 0x07, 0x00);
    ES7242_WriteReg(ES7242_Address1, 0x08, 0x11);
    ES7242_WriteReg(ES7242_Address1, 0x09, 0x00);

    ES7242_WriteReg(ES7242_Address2, 0x01, 0x8F);
    ES7242_WriteReg(ES7242_Address2, 0x07, 0x00);
    ES7242_WriteReg(ES7242_Address2, 0x08, 0x11);
    ES7242_WriteReg(ES7242_Address2, 0x09, 0x00);
#endif
#if 1
    ES7242_WriteReg(ES7242_Address3, 0x01, 0x0F);
    ES7242_WriteReg(ES7242_Address3, 0x07, 0x00);
    ES7242_WriteReg(ES7242_Address3, 0x08, 0x11);
    ES7242_WriteReg(ES7242_Address3, 0x09, 0x00);
#endif
#if 1
    ES7242_WriteReg(ES7242_Address4, 0x01, 0x8F);
    ES7242_WriteReg(ES7242_Address4, 0x07, 0x00);
    ES7242_WriteReg(ES7242_Address4, 0x08, 0x11);
    ES7242_WriteReg(ES7242_Address4, 0x09, 0x00);
#endif
}

void ES7242_ADC_StandBy()
{


}
void ES7242_init(void)
{
    //I2C_MasterInit(0x0);
    i2c_master_gpio_init();
    ES8388_PowerOn_Init1();
    ES8388_DAC_Play1();
    //ES7242_Poweron_Init();
   // ES7242_ADC_Record();

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
            i2c_master_wait(500);  // Wait 500us and retry.
            i2c_master_start();
        } else {
            return true;
        }
    }
    i2c_master_stop();
    return false;
}
#if 1
void ES7242_WriteReg(uint8_t slave_add, uint8_t reg_add, uint8_t data)
{
    i2c_master_start();
    if (false == I2C_Write_Byte(slave_add, 10)) {
        ets_printf("Slave is busy, TIME OUT");
    }
    if (false == I2C_Write_Byte(reg_add, 10)) {
        ets_printf("Slave is busy, TIME OUT");
    }
    if (false == I2C_Write_Byte(data, 10)) {
        ets_printf("Slave is busy, TIME OUT");
    }
    i2c_master_stop();
}
#endif
#if 0
void ES7242_WriteReg(uint8_t slave_add, uint8_t reg_add, uint8_t data)
{
    i2c_send_data_test(0x0, slave_add, reg_add, data);
//ets_delay_us(10000);
}
#endif
uint8_t ES7242_ReadReg(uint8_t slave_add, uint8_t reg_add, uint8_t dat)
{
    i2c_master_start();
    if (false == I2C_Write_Byte(slave_add, 0)) {
        printf("Slave is busy, TIME OUT");
    }
    if (false == I2C_Write_Byte(reg_add, 0)) {
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
