/*
 * ESPRSSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP32 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */
#if 0
#include <stdio.h>
//#include "esp_common.h"
//#include "i2c_test.h"
//#include "i2c_register.h"
#include "../gpio/gpio.h"
//#include "../test_code/peripheral_test.h"
#include "i2c.h"

#define FIELD_MODE   1

#define I2C_FREQ    100000   //HZ
#define I2C_SLAVE_DEV_ADDR 0x60  

#define I2C_SDA_OUT_IO_NUM   19
#define I2C_SDA_OUT_IO_SIG   I2CEXT0_SDA_OUT_IDX
#define I2C_SDA_OUT_IO_PIN   GPIO_Pin_19

#define I2C_SCL_OUT_IO_NUM   23
#define I2C_SCL_OUT_IO_SIG   I2CEXT0_SCL_OUT_IDX
#define I2C_SCL_OUT_IO_PIN   GPIO_Pin_23

#define I2C_SDA_IN_IO_SIG   I2CEXT0_SDA_IN_IDX
#define I2C_SCL_IN_IO_SIG    I2CEXT0_SCL_IN_IDX

void I2C_GpioInit()
{
	gpio_config_t gpio_conf;
	gpio_conf.GPIO_Mode = GPIO_Mode_Input_OutOD;
	gpio_conf.GPIO_Pin = I2C_SDA_OUT_IO_PIN | I2C_SCL_OUT_IO_PIN ;
	gpio_conf.GPIO_Pulldown = GPIO_PullDown_DIS;
	gpio_conf.GPIO_Pullup = GPIO_PullUp_EN;
	gpio_conf.GPIO_IntrType = GPIO_PIN_INTR_DISABLE;
	gpio_config(&gpio_conf);

	gpio_matrix_out(I2C_SDA_OUT_IO_NUM,I2C_SDA_OUT_IO_SIG,0,0);
	gpio_matrix_out(I2C_SCL_OUT_IO_NUM, I2C_SCL_OUT_IO_SIG,0,0);
	gpio_matrix_in(I2C_SDA_OUT_IO_NUM, I2C_SDA_IN_IO_SIG,0);
	gpio_matrix_in(I2C_SCL_OUT_IO_NUM, I2C_SCL_IN_IO_SIG,0);
}
#if 0
    // SDA OUT
    uint32 i2c_sda_io_mux_reg = GPIO_PIN_MUX_REG[I2C_SDA_OUT_IO_NUM];
	gpio_matrix_out(I2C_SDA_OUT_IO_NUM,I2C_SDA_OUT_IO_SIG);
	PIN_FUNC_SELECT(i2c_sda_io_mux_reg, I2C_GPIO_FUNC);
	SET_PERI_REG_MASK(i2c_sda_io_mux_reg,FUN_IE|FUN_PU);
	CLEAR_PERI_REG_MASK(i2c_sda_io_mux_reg,FUN_PD);
	SET_PERI_REG_MASK(i2c_sda_io_mux_reg,GPIO_PIN_PAD_DRIVER);
    //SDL OUT
    uint32 i2c_scl_io_mux_reg = GPIO_PIN_MUX_REG[I2C_SCL_OUT_IO_NUM];
	gpio_matrix_out(I2C_SCL_OUT_IO_NUM, I2C_SCL_OUT_IO_SIG);
	PIN_FUNC_SELECT(i2c_scl_io_mux_reg, I2C_GPIO_FUNC);
	SET_PERI_REG_MASK(i2c_scl_io_mux_reg,FUN_IE|FUN_PU);
	CLEAR_PERI_REG_MASK(i2c_scl_io_mux_reg,FUN_PD);
	SET_PERI_REG_MASK(i2c_scl_io_mux_reg,GPIO_PIN_PAD_DRIVER);
	//SDA IN
	gpio_matrix_in(I2C_SDA_OUT_IO_NUM, I2C_SDA_IN_IO_SIG);
	//SCL IN
	gpio_matrix_in(I2C_SCL_OUT_IO_NUM, I2C_SCL_IN_IO_SIG);
#endif
	return 0;
}


void i2c_send_data_test(uint8 i2c_no,uint8 mask1,uint8 mask2, uint8 mask3)
{
    uint8 data_buf[] = {0xa8,  //slave addr
		                0x13,  //reg addr
                        mask1, //val1
                        mask2, //val2
                        mask3, //val3
                        0      //val4
    	                };

    uint8 byte_num = sizeof(data_buf);
    struct I2C_CmdDef cmd;
    os_memset(&cmd,0,sizeof(cmd));

    //reset tx fifo
    I2C_ResetTxFifo(I2C(i2c_no));

    //setup command 0
	cmd.ack_en = 0;
	cmd.ack_exp = 1;
	cmd.ack_val = 1;
	cmd.byte_num = 0;
	cmd.op_code = I2C_CMD_RESTART;
	I2C_ConfigCmd(i2c_no,0, &cmd);

	//setup command 1
	cmd.byte_num = byte_num;
	cmd.op_code = I2C_CMD_WRITE;
	I2C_ConfigCmd(i2c_no,1, &cmd);

	//setup command 2
    cmd.byte_num = 0;
    cmd.op_code = I2C_CMD_STOP;
    I2C_ConfigCmd(i2c_no,2, &cmd);

    //push the sending data to tx fifo
    I2C_TxFifoPush(i2c_no,data_buf ,byte_num);

    //I2C start sending
    I2C_Start(i2c_no);
}



void i2c_intr_handler(void)
{
    uint8 i2c_no = 0;  //UART1

    I2C_TypeDef* I2Cx = I2C(i2c_no);
//    uint32 i2c_intr_status = READ_PERI_REG(I2C_I2C_INT_STATUS_REG(i2c_no));
    uint32 i2c_intr_status = I2Cx->I2C_INT_STATUS.val;

	//uint16 fifo_len = I2C_GET_RXFIFO_CNT(i2c_no);
	uint16 rx_fifo_len = I2Cx->SR.fld.rxfifo_cnt;
	//uint16 tx_fifo_len = I2C_GET_TXFIFO_CNT(i2c_no);
	uint16 tx_fifo_len = I2Cx->SR.fld.txfifo_cnt;

    uint16 i = 0;
	os_printf("====================================================\n");
	os_printf("IN I2C RX INTR !!!!!!!!\r\n");
	os_printf("i2c_intr_status: 0x%08x \r\n",i2c_intr_status);



    if( (i2c_intr_status >> I2C_RXFIFO_FULL_INT_ST_S)&I2C_RXFIFO_FULL_INT_ST_V){
        os_printf("I2C_RXFIFO_FULL_INT, rx_fifo_len: %d\r\n",rx_fifo_len);
		for(i=0;i<rx_fifo_len;i++){
            //os_printf("%c",I2C_GET_FIFO_RDATA(i2c_no));
            os_printf("%c",I2Cx->I2C_DATA.fld.fifo_data);
		}
    }
	else if( (i2c_intr_status >> I2C_TXFIFO_EMPTY_INT_ST_S)&I2C_TXFIFO_EMPTY_INT_ST_V){
        os_printf("I2C_TXFIFO_EMPTY_INT, TX_fifo_len: %d\r\n",tx_fifo_len);
//        I2C_SET_TXFIFO_EMPTY_INT_ENA(i2c_no,0);
        I2Cx->I2C_INT_ENA.fld.tx_send_empty_int_ena = 0;
    }
	else if( (i2c_intr_status >> I2C_RXFIFO_OVF_INT_ST_S)&I2C_RXFIFO_OVF_INT_ST_V){
        os_printf("I2C_RXFIFO_OVF_INT, tx_fifo_len: %d\r\n",tx_fifo_len);
		for(i=0;i<rx_fifo_len;i++){
            os_printf("%c",I2Cx->I2C_DATA.fld.fifo_data);
		}
    }
	else if( (i2c_intr_status >> I2C_SLAVE_TRAN_COMP_INT_ST_S)&I2C_SLAVE_TRAN_COMP_INT_ST_V){
        os_printf("I2C_SLAVE_TRAN_COMP_INT_ST, tx_fifo_len: %d\r\n",tx_fifo_len);
    }
	else if( (i2c_intr_status >> I2C_ARBITRATION_LOST_INT_ST_S)&I2C_ARBITRATION_LOST_INT_ST_V){
        os_printf("I2C_ARBITRATION_LOST_INT_ST, tx_fifo_len: %d\r\n",tx_fifo_len);
    }
	else if( (i2c_intr_status >> I2C_MASTER_TRAN_COMP_INT_ST_S)&I2C_MASTER_TRAN_COMP_INT_ST_V){
        os_printf("I2C_MASTER_TRAN_COMP_INT_ST, tx_fifo_len: %d\r\n",tx_fifo_len);
    }
	else if( (i2c_intr_status >> I2C_TRANS_COMPLETE_INT_ST_S)&I2C_TRANS_COMPLETE_INT_ST_V){
        os_printf("I2C_TRANS_COMPLETE_INT_ST, tx_fifo_len: %d\r\n",tx_fifo_len);
    }
	else if( (i2c_intr_status >> I2C_TIME_OUT_INT_ST_S)&I2C_TIME_OUT_INT_ST_V){
        os_printf("I2C_TIME_OUT_INT_ST, tx_fifo_len: %d\r\n",tx_fifo_len);
		os_printf("DISABLE TOUT INT...time out status: %d \r\n",I2Cx->SR.fld.time_out);
//		I2C_SET_TIME_OUT_INT_ENA(i2c_no,0);
		I2Cx->I2C_INT_ENA.fld.time_out_int_ena = 0;
    }
	else if( (i2c_intr_status >> I2C_TRANS_START_INT_ST_S)&I2C_TRANS_START_INT_ST_V){
        os_printf("I2C_TRANS_START_INT_ST, fifo_len: %d\r\n",tx_fifo_len);
    }
	//uart_intr_status = READ_PERI_REG(UART_INT_ST_REG(uart_no)) ;
//    SET_PERI_REG_MASK(I2C_I2C_INT_CLR_REG(i2c_no),i2c_intr_status);
    I2Cx->I2C_INT_CLR.val = i2c_intr_status;
}



void I2C_IntrConfig(uint8 i2c_no)
{
	I2C_TypeDef* I2Cx = I2C(i2c_no);
    os_printf("===========================\r\n");
	os_printf("I2C0 TEST INTR CONFIG: %d\r\n",i2c_no);
    os_printf("===========================\r\n");
    I2Cx->I2C_INT_CLR.val = 0xffffffff;
    ETS_INTR_DISABLE(ETS_I2C0_INUM);
	//REG_SET_FIELD(PRO_UART1_INTR_MAP_REG,DPORT_PRO_UART1_INTR_MAP,ETS_UART1_INUM);
    //REG_SET_FIELD(PRO_UART1_INTR_MAP_REG,DPORT_PRO_UART1_INTR_MAP,ETS_UART1_INUM);
    //ETS_INTR_ATTACH(i2c_rx_intr_handler, NULL,ETS_I2C0_INUM);
    //ETS_INTR_ENABLE(ETS_I2C0_INUM);

    os_printf("EN INTR.\n");
    I2C_IntrRegister(i2c_intr_handler,NULL);
    I2Cx->I2C_INT_ENA.val = 0;
    os_printf("I2C INT EN0: 0X%08x\n",READ_PERI_REG(0x60013028));
    //I2Cx->I2C_INT_ENA.fld.master_tran_comp_int_ena = 1;
    I2Cx->I2C_INT_ENA.fld.time_out_int_ena =1;
    I2Cx->I2C_INT_ENA.fld.trans_complete_int_ena=1;
    I2Cx->I2C_INT_ENA.fld.trans_start_int_ena=1;
    //WRITE_PERI_REG(0x60013028,0x3c0);
    os_printf("I2C INT EN01: 0X%08x\n",READ_PERI_REG(0x60013028));
    ETS_INTR_ENABLE(ETS_I2C0_INUM);
}

void I2C_MasterTest(uint8 i2c_no)
{
	os_printf("test i2c master...\r\n");
	os_printf("gpio init");
	//step 1: gpio init
	I2C_GpioInit();

	//step 2: i2c init
    //master init
	I2C_InitTypeDef I2C_Init_t;
	I2C_Init_t.addr_10bit_en = 0;       //not used
	I2C_Init_t.clk_speed = 100000;      //clk set
	I2C_Init_t.mode = I2C_MASTER_MODE;  //master mode
	I2C_Init_t.slave_addr = 0x0;        //no slave addr for master mode
	I2C_Init(i2c_no,&I2C_Init_t);
	//I2C_IntrConfig(i2c_no);
#if 0
	int i =1;
	uint32 m_tmp = (1<<(i%18));
	uint8 mask1 = (m_tmp>>0) &0x3f ;
	uint8 mask2 = (m_tmp>>6) &0x3f ;
	uint8 mask3 = (m_tmp>>12) &0x3f ;
	i2c_send_data_test(0,mask1, mask2, mask3);
	os_printf("finish sending..\r\n");
#endif
}

void i2c_send_long_data_test()
{
	os_printf("send long data\n");
	uint8 i2c_no = 0;
	uint8 cmd_idx = 0;
	I2C_TypeDef* I2Cx = I2C(i2c_no);

	int i =1;
	uint8 data_buf[256];
	for(i=0;i< sizeof(data_buf);i++) data_buf[i]=i;

	struct I2C_CmdDef cmd;
	os_memset(&cmd,0,sizeof(cmd));

	//reset tx fifo
	I2C_ResetTxFifo(i2c_no);

	//setup command 0: start
	cmd_idx = 0;
	cmd.ack_en = 0;
	cmd.ack_exp = 1;
	cmd.ack_val = 1;
	cmd.byte_num = 0;
	cmd.op_code = I2C_CMD_RESTART;
	I2C_ConfigCmd(i2c_no,cmd_idx, &cmd);

	//setup command 1: write
	cmd_idx = 1;
	cmd.byte_num = 31;
	cmd.op_code = I2C_CMD_WRITE;
	I2C_ConfigCmd(i2c_no,cmd_idx, &cmd);

	//setup command 2: end
	cmd_idx = 2;
	cmd.byte_num = 0;
	cmd.op_code = I2C_CMD_END;
	I2C_ConfigCmd(i2c_no,cmd_idx,&cmd);
	I2C_TxFifoPush(i2c_no,data_buf ,31);

//	os_printf("tcmd1.done:%d\n",I2Cx->I2C_COMD[1].fld.command_done);
//	os_printf("tcmd2.done:%d\n",I2Cx->I2C_COMD[2].fld.command_done);
	I2C_Start(i2c_no);
    os_printf("2");
    uint8 val;
	while(1){
		if(I2Cx->I2C_COMD[1].fld.command_done == 1) break;
		//os_printf("dne:%d\n",I2Cx->I2C_COMD[1].fld.command_done);
		//if(I2C_GET_COMMAND_DONE(0,1) == 1) break;
		ets_delay_us(1);
	}
//	os_printf("send 1st\n");
//    ets_delay_us(50000);
//    os_printf("cmd1.done2:%d\n",I2Cx->I2C_COMD[1].fld.command_done);
//    os_printf("cmd2.done2:%d\n",I2Cx->I2C_COMD[2].fld.command_done);
//    os_printf("send 2nd\n");

	cmd_idx = 0;
	cmd.byte_num = 31;
	cmd.op_code = I2C_CMD_WRITE;
	I2C_ConfigCmd(i2c_no,cmd_idx, &cmd);
	I2C_TxFifoPush(i2c_no,data_buf+31 ,31);


	//setup command 0: stop
	cmd_idx = 1;
	cmd.byte_num = 0;
	cmd.op_code = I2C_CMD_STOP;
	I2C_ConfigCmd(i2c_no,cmd_idx, &cmd);

	//push the sending data to tx fifo
	//I2C_TxFifoPush(i2c_no,data_buf ,byte_num);

	//I2C start sending
	I2C_Start(i2c_no);

}


#if 0
void I2C1_GPIO_Init()
{
#if 1
    // SDA OUT
	gpio_matrix_out(21, I2C1_SDA_OUT_IO_SIG);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO21_U, FUNC_GPIO21_GPIO21);
	SET_PERI_REG_MASK(PERIPHS_IO_MUX_GPIO21_U,FUN_IE|FUN_PU);
	CLEAR_PERI_REG_MASK(PERIPHS_IO_MUX_GPIO21_U,FUN_PD);
    SET_PERI_REG_MASK(GPIO_PIN21_REG,GPIO_PIN21_PAD_DRIVER);
//====debug====
	gpio_matrix_out(22, I2C1_SCL_OUT_IO_SIG);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO22_U, FUNC_GPIO22_GPIO22);
	SET_PERI_REG_MASK(PERIPHS_IO_MUX_GPIO22_U,FUN_IE|FUN_PU);
	CLEAR_PERI_REG_MASK(PERIPHS_IO_MUX_GPIO22_U,FUN_PD);
    SET_PERI_REG_MASK(GPIO_PIN22_REG,GPIO_PIN22_PAD_DRIVER);
//============================
    //SDA IN
    gpio_matrix_in(21, I2C1_SDA_IN_IO_SIG);
	//SCL IN
	gpio_matrix_in(22, I2C1_SCL_IN_IO_SIG);
#endif
}

#endif

#if 0
void I2C_GPIO_Init_ori()
{
#if 1
    // SDA OUT
	gpio_matrix_out(19, I2C_SDA_OUT_IO_SIG);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO19_U, FUNC_GPIO19_GPIO19);
	SET_PERI_REG_MASK(PERIPHS_IO_MUX_GPIO19_U,FUN_IE|FUN_PU);
	CLEAR_PERI_REG_MASK(PERIPHS_IO_MUX_GPIO19_U,FUN_PD);
    SET_PERI_REG_MASK(GPIO_PIN19_REG,GPIO_PIN19_PAD_DRIVER);
//====debug====
	gpio_matrix_out(20, I2C_SCL_OUT_IO_SIG);
    PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO20_U, FUNC_GPIO20_GPIO20);
	SET_PERI_REG_MASK(PERIPHS_IO_MUX_GPIO20_U,FUN_IE|FUN_PU);
	CLEAR_PERI_REG_MASK(PERIPHS_IO_MUX_GPIO20_U,FUN_PD);
    SET_PERI_REG_MASK(GPIO_PIN20_REG,GPIO_PIN20_PAD_DRIVER);
//============================
    //SDA IN
    gpio_matrix_in(19, I2C_SDA_IN_IO_SIG);
	//SCL IN
	gpio_matrix_in(20, I2C_SCL_IN_IO_SIG);
#endif

    //init I2C1 GPIO

}



void i2c_slave_init(uint8 i2c_no)
{
    if(i2c_no>1){
        os_printf("invalid i2c number...\r\n");
		return;
    }
	
    I2C1_GPIO_Init();

	
    //set master mode
    //I2C_SET_RX_LSB_FIRST(i2c_no,0); //MSB FIRST
    //I2C_SET_TX_LSB_FIRST(i2c_no,0); //MSB FIRST
    
    I2C_SET_MS_MODE(i2c_no,0); //SLAVE MODE
    //I2C_SET_SCL_FORCE_OUT(i2c_no,1); //out put practical level
    //I2C_SET_SDA_FORCE_OUT(i2c_no,1); //out put practical level

	I2C_SET_SLAVE_ADDR(i2c_no,I2C_SLAVE_DEV_ADDR);
	I2C_SET_ADDR_10BIT_EN(i2c_no,0);
	

	//SET FREQ/DUTY
	I2C_SET_SCL_LOW_PERIOD(i2c_no, (APB_CLK_FREQ/I2C_FREQ)/2);
	I2C_SET_SCL_HIGH_PERIOD(i2c_no, (APB_CLK_FREQ/I2C_FREQ)/2);

	//SET CLK RE/START HOLD/SETUP
	I2C_SET_SCL_START_HOLD_TIME(i2c_no,50);//400);
	I2C_SET_SCL_RSTART_SETUP_TIME(i2c_no,50);//400);

	//SET CLK STOP HOLD/SETUP
	I2C_SET_SCL_STOP_HOLD_TIME(i2c_no,50);//400);
	I2C_SET_SCL_STOP_SETUP_TIME(i2c_no,50);//400);

	//SET DATA I/O HOLD/SAMPLE
	I2C_SET_SDA_HOLD_TIME(i2c_no,40);
	I2C_SET_SDA_SAMPLE_TIME(i2c_no,40);

	//SET CLK TIME OUT
	I2C_SET_TIME_OUT_THRSH(i2c_no,2000);

	//SET FIFO MODE
	//I2C_SET_NONFIFO_EN(i2c_no,0);
	//SET SCL FILTER
	//I2C_SET_SCL_FILTER_EN(i2c_no,1);
	//SET SDA FILTER
	//I2C_SET_SDA_FILTER_EN(i2c_no,0);
	
    os_printf("===========================\r\n");
	os_printf("I2C1 SLAVE TRANS START\r\n");
    os_printf("===========================\r\n");
	I2C_SET_TRANS_START(i2c_no,1);


	
    os_printf("===========================\r\n");
	os_printf("I2C1 TEST INTR CONFIG\r\n");
    os_printf("===========================\r\n");
    ETS_INTR_DISABLE(ETS_I2C1_INUM);
	//REG_SET_FIELD(PRO_UART1_INTR_MAP_REG,DPORT_PRO_UART1_INTR_MAP,ETS_UART1_INUM);
    intr_matrix_set(ETS_I2C1_SOURCE, ETS_I2C1_INUM);
    ETS_INTR_ATTACH(i2c1_rx_intr_handler, NULL,ETS_I2C1_INUM);
    ETS_INTR_ENABLE(ETS_I2C1_INUM);


}


void i2c_master_init(uint8 i2c_no)
{
    if(i2c_no>1){
        os_printf("invalid i2c number...\r\n");
		return;
    }
	
    //set master mode
    //I2C_SET_RX_LSB_FIRST(i2c_no,0); //MSB FIRST
    //I2C_SET_TX_LSB_FIRST(i2c_no,0); //MSB FIRST
    I2C_SET_MS_MODE(i2c_no,1); //MASTER MODE
    //I2C_SET_SCL_FORCE_OUT(i2c_no,1); //out put practical level
    //I2C_SET_SDA_FORCE_OUT(i2c_no,1); //out put practical level

	//SET FREQ/DUTY
	I2C_SET_SCL_LOW_PERIOD(i2c_no, (APB_CLK_FREQ/I2C_FREQ)/2);
	I2C_SET_SCL_HIGH_PERIOD(i2c_no, (APB_CLK_FREQ/I2C_FREQ)/2);

	//SET CLK RE/START HOLD/SETUP
	I2C_SET_SCL_START_HOLD_TIME(i2c_no,50);//400);
	I2C_SET_SCL_RSTART_SETUP_TIME(i2c_no,50);//400);

	//SET CLK STOP HOLD/SETUP
	I2C_SET_SCL_STOP_HOLD_TIME(i2c_no,50);//400);
	I2C_SET_SCL_STOP_SETUP_TIME(i2c_no,50);//400);

	//SET DATA I/O HOLD/SAMPLE
	I2C_SET_SDA_HOLD_TIME(i2c_no,40);
	I2C_SET_SDA_SAMPLE_TIME(i2c_no,40);

	//SET CLK TIME OUT
	I2C_SET_TIME_OUT_THRSH(i2c_no,2000);

	//SET FIFO MODE
	//I2C_SET_NONFIFO_EN(i2c_no,0);
	//SET SCL FILTER
	//I2C_SET_SCL_FILTER_EN(i2c_no,1);
	//SET SDA FILTER
	//I2C_SET_SDA_FILTER_EN(i2c_no,0);

	i2c_intr_config();
}


void i2c_master_read_test()
{
    uint8 i2c_no = 0;
    uint8 idx = 0;
	
    uint8 data_buf[] = {I2C_SLAVE_DEV_ADDR, //addr
                        0x10,
                        0x01,
                        0x12,
                        0x13,
                        0x14,
    	                };
    uint8 byte_num = sizeof(data_buf);
	
	I2C_SET_TX_FIFO_RST(0,1);
	I2C_SET_TX_FIFO_RST(0,0);

	//command0 : sending restart signal
	idx = 0;
	I2C_SET_COMMAND_OP_CODE(i2c_no,idx,0);
	I2C_SET_COMMAND_ACK_EN(i2c_no,idx,0);
	I2C_SET_COMMAND_BYTE_NUM(i2c_no,0,1);

	//command1 : picking slave and sending data
	idx = 1;
	I2C_SET_COMMAND_ACK_EN(i2c_no,idx,0);
	I2C_SET_COMMAND_OP_CODE(i2c_no,idx,1);//WRITE COMMAND
	I2C_SET_COMMAND_BYTE_NUM(i2c_no,idx,byte_num); //BYTE NUMBER

	//command2 : master generating stop 
	idx = 2;
	I2C_SET_COMMAND_OP_CODE(i2c_no,idx,0x3); //STOP
	I2C_SET_COMMAND_ACK_EN(i2c_no,idx,0); //NO ACK

	int i = 0;
	for(i=0;i<byte_num;i++){
		WRITE_PERI_REG(I2C_I2C_DATA_REG(i2c_no),data_buf[i]);
	}

	//start sending
	I2C_SET_TRANS_START(i2c_no,1);
}



void i2c_master_write_long_data(uint8  i2c_no)
{
    uint8 idx,idx2 = 0;
	uint8 data_buf[] = {1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6,7,8,9,0};
	uint8 byte_num = sizeof(data_buf);
	
	//command0 : sending restart signal
	idx = 0;
	os_printf("IDX 0: %d \r\n",idx);
	I2C_SET_COMMAND_DONE(i2c_no,idx,0);
	I2C_SET_COMMAND_OP_CODE(i2c_no,idx,0);
	I2C_SET_COMMAND_ACK_EN(i2c_no,idx,0);
	I2C_SET_COMMAND_BYTE_NUM(i2c_no,idx,1);

	//command1 : picking slave and sending data
	idx = 1;
	
	os_printf("IDX 1: %d \r\n",idx);
	I2C_SET_COMMAND_DONE(i2c_no,idx,0);
	I2C_SET_COMMAND_ACK_EN(i2c_no,idx,0);
	I2C_SET_COMMAND_OP_CODE(i2c_no,idx,1);//WRITE COMMAND
	I2C_SET_COMMAND_BYTE_NUM(i2c_no,idx,byte_num); //BYTE NUMBER

	//command2 : master generating END
	idx = 2;
	os_printf("IDX 2: %d \r\n",idx);
	I2C_SET_COMMAND_DONE(i2c_no,idx,0);
	I2C_SET_COMMAND_OP_CODE(i2c_no,idx,0x4); //END
	I2C_SET_COMMAND_ACK_EN(i2c_no,idx,0); //NO ACK

	int i = 0;
	for(i=0;i<byte_num;i++){
		WRITE_PERI_REG(I2C_I2C_DATA_REG(i2c_no),data_buf[i]);
	}

	I2C_SET_TRANS_START(i2c_no,1);

	//os_printf("trans check...\r\n");
	while( I2C_GET_COMMAND_DONE(i2c_no,1) == 0);
	//os_printf("trans check ok...\r\n");

	//command0 : sending restart signal
	//idx=0;
	//I2C_SET_COMMAND_DONE(i2c_no,idx,0);
	//I2C_SET_COMMAND_OP_CODE(i2c_no,idx,0);
	//I2C_SET_COMMAND_ACK_EN(i2c_no,idx,0);
	//I2C_SET_COMMAND_BYTE_NUM(i2c_no,idx,1);

	//command1 : picking slave and sending data
	idx = 0;
	//os_printf("IDX2 0: %d \r\n",idx);
	I2C_SET_COMMAND_DONE(i2c_no,idx,0);
	
	I2C_SET_COMMAND_ACK_EXP(i2c_no,idx,1);
	I2C_SET_COMMAND_ACK_EN(i2c_no,idx,0);
	I2C_SET_COMMAND_OP_CODE(i2c_no,idx,1);//WRITE COMMAND
	I2C_SET_COMMAND_BYTE_NUM(i2c_no,idx,2); //BYTE NUMBER
	//os_printf("******************\r\n");
    //os_printf("command %d : 0x%08x   0x%08x\r\n",idx,READ_PERI_REG(I2C_I2C_COMD0_REG(i2c_no)),I2C_GET_COMMAND_OP_CODE(i2c_no,idx));

	idx = 1;
	//os_printf("IDX2 1: %d \r\n",idx);
	I2C_SET_COMMAND_DONE(i2c_no,idx,0);
	
	I2C_SET_COMMAND_ACK_EXP(i2c_no,idx,1);
	I2C_SET_COMMAND_ACK_EN(i2c_no,idx,0);
	
	I2C_SET_COMMAND_OP_CODE(i2c_no,idx,1);//WRITE COMMAND
	I2C_SET_COMMAND_BYTE_NUM(i2c_no,idx,2); //BYTE NUMBER

	//command2 : master generating stop 
	//idx = 2;	
	idx = 2; //why ??? if no this line, idx is last value not 2
	//os_printf("IDX2 2: %d \r\n",idx);


	I2C_SET_COMMAND_DONE(i2c_no,idx,0);
	I2C_SET_COMMAND_OP_CODE(i2c_no,idx,0x3); //STOP
	I2C_SET_COMMAND_ACK_EN(i2c_no,idx,0); //NO ACK
	//os_printf("******************\r\n");
    //os_printf("command %d : 0x%08x   0x%08x\r\n",idx,READ_PERI_REG(I2C_I2C_COMD1_REG(i2c_no)),I2C_GET_COMMAND_OP_CODE(i2c_no,idx));

	for(i=0;i<byte_num;i++){
		WRITE_PERI_REG(I2C_I2C_DATA_REG(i2c_no),data_buf[i]);
	}
	I2C_SET_TRANS_START(i2c_no,1);

}



void i2c_write_led_test(uint8 i2c_no,uint8 mask1,uint8 mask2, uint8 mask3)
{
    uint8 data_buf[] = {0xa8,
		                0x13, //addr
                        mask1,
                        mask2,
                        mask3,
                        0 //update
    	                };
    uint8 byte_num = sizeof(data_buf);
	
	I2C_SET_TX_FIFO_RST(0,1);
	I2C_SET_TX_FIFO_RST(0,0);
	
    //command0 : sending restart signal
    I2C_SET_COMMAND_OP_CODE(i2c_no,0,0);
	I2C_SET_COMMAND_ACK_EN(i2c_no,0,0);
	I2C_SET_COMMAND_BYTE_NUM(i2c_no,0,1);

	//command1 : picking slave and sending data
    I2C_SET_COMMAND_ACK_EN(i2c_no,1,0);
	I2C_SET_COMMAND_OP_CODE(i2c_no,1,1);//WRITE COMMAND
	I2C_SET_COMMAND_BYTE_NUM(i2c_no,1,byte_num); //BYTE NUMBER

	//command2 : master generating stop 
	I2C_SET_COMMAND_OP_CODE(i2c_no,2,0x3); //STOP
    I2C_SET_COMMAND_ACK_EN(i2c_no,2,0); //NO ACK

	
    int i = 0;
    for(i=0;i<byte_num;i++){
    	WRITE_PERI_REG(I2C_I2C_DATA_REG(i2c_no),data_buf[i]);
    }
    
    //start sending
    I2C_SET_TRANS_START(i2c_no,1);

}

#if 1
void i2c_master_tx_cfg(uint8 i2c_no)
{
    //SET CMD & ACK
    //I2C_SET_COMMAND0(i2c_no,)

    uint8 data_buf[] = {0xa8,
                        0x0,  //addr
                        0x1,  //normal mode
		                0XFF,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff, //data
		                0x3f,0x0,0x3f,0x01};
    int byte_num = sizeof(data_buf);

    I2C_SET_TX_FIFO_RST(i2c_no,1);
    I2C_SET_TX_FIFO_RST(i2c_no,0);

	//I2C_SET_SLAVE_ADDR(i2c_no,0xa8);
	//I2C_SET_ADDR_10BIT_EN(i2c_no,0);
	
    //command0 : sending restart signal
    I2C_SET_COMMAND_OP_CODE(i2c_no,0,0);
	I2C_SET_COMMAND_ACK_EN(i2c_no,0,0);
	I2C_SET_COMMAND_BYTE_NUM(i2c_no,0,1);

	//command1 : picking slave and sending data
    I2C_SET_COMMAND_ACK_EN(i2c_no,1,0);
	I2C_SET_COMMAND_OP_CODE(i2c_no,1,1);//WRITE COMMAND
	I2C_SET_COMMAND_BYTE_NUM(i2c_no,1,byte_num); //BYTE NUMBER

	//command2 : master generating stop 
	I2C_SET_COMMAND_OP_CODE(i2c_no,2,0x3); //STOP
    I2C_SET_COMMAND_ACK_EN(i2c_no,2,0); //NO ACK

	//command3: start 
    //I2C_SET_COMMAND_OP_CODE(i2c_no,2,0);
	//I2C_SET_COMMAND_ACK_EN(i2c_no,2,0);
	//I2C_SET_COMMAND_BYTE_NUM(i2c_no,2,1);
	//command4: stop 
	//I2C_SET_COMMAND_OP_CODE(i2c_no,3,0x3); //STOP
    //I2C_SET_COMMAND_ACK_EN(i2c_no,3,0); //NO ACK

	
	//write fifo
	int i = 0;
	for(i=0;i<byte_num;i++){
        WRITE_PERI_REG(I2C_I2C_DATA_REG(i2c_no),data_buf[i]);
	}
	
	//start sending
	I2C_SET_TRANS_START(i2c_no,1);
	for(i = 0;i<50;i++){

		//while(I2C_GET_TRANS_COMPLETE_INT_RAW(0)==0);
		while(I2C_GET_COMMAND2_DONE(0) == 0);
		//I2C_SET_TRANS_COMPLETE_INT_CLR(0,1);
		
		ets_delay_us(5000);
		ets_delay_us(5000);
		ets_delay_us(5000);
		ets_delay_us(5000);	
		
        uint32 m_tmp = (1<<(i%18));
		uint8 mask1 = (m_tmp>>0) &0x3f ;
		uint8 mask2 = (m_tmp>>6) &0x3f ;
		uint8 mask3 = (m_tmp>>12) &0x3f ;
		i2c_write_led_test(i2c_no,mask1,mask2, mask3);
	}
}
#else


void i2c_master_tx_cfg(uint8 i2c_no)
{
    //SET CMD & ACK
    //I2C_SET_COMMAND0(i2c_no,)

    uint8 data_buf[] = {0x0,  //addr
                        0x1,  //normal mode
		                0XFF,0x1f,0xff,0x1f,0xff,0x1f,0xff,0x1f,0xff,0x1f,0xff,0x1f,0xff,0xff,0xff,0xff,0xff,0xff, //data
		                0x3f,0x0,0x3f,0x01};
    int byte_num = sizeof(data_buf);

    I2C_SET_TX_FIFO_RST(i2c_no,1);
    I2C_SET_TX_FIFO_RST(i2c_no,0);

	//I2C_SET_SLAVE_ADDR(i2c_no,0xa8);
	//I2C_SET_ADDR_10BIT_EN(i2c_no,0);
	
    //command0 : sending restart signal
    I2C_SET_COMMAND_OP_CODE(i2c_no,0,0);
	I2C_SET_COMMAND_ACK_EN(i2c_no,0,0);
	I2C_SET_COMMAND_BYTE_NUM(i2c_no,0,1);

	//command1 : picking slave and sending data
    //I2C_SET_COMMAND_ACK_EN(i2c_no,1,0);
	//I2C_SET_COMMAND_OP_CODE(i2c_no,1,1);//WRITE COMMAND
	//I2C_SET_COMMAND_BYTE_NUM(i2c_no,1,byte_num+1); //BYTE NUMBER

	//command2 : master generating stop 
	I2C_SET_COMMAND_OP_CODE(i2c_no,1,0x3); //STOP
    I2C_SET_COMMAND_ACK_EN(i2c_no,1,0); //NO ACK

	//command3: start 
    //I2C_SET_COMMAND_OP_CODE(i2c_no,2,0);
	//I2C_SET_COMMAND_ACK_EN(i2c_no,2,0);
	//I2C_SET_COMMAND_BYTE_NUM(i2c_no,2,1);
	//command4: stop 
	//I2C_SET_COMMAND_OP_CODE(i2c_no,3,0x3); //STOP
    //I2C_SET_COMMAND_ACK_EN(i2c_no,3,0); //NO ACK
	
	//write fifo
	WRITE_PERI_REG(I2C_I2C_DATA_REG(i2c_no),0xa8);  //SLAVE ADDRESS: 7BIT ADDR + R/W BIT
	int i = 0;
	for(i=0;i<byte_num;i++){
        //WRITE_PERI_REG(I2C_I2C_DATA_REG(i2c_no),data_buf[i]);
	}
	
	//start sending
	I2C_SET_TRANS_START(i2c_no,1);



}

#endif

#if 1
#define I2C_INT_STR  "I2C0 INTR:"
void i2c_rx_intr_handler()
{
    uint8 i2c_no = 0;  //UART1
    uint32 i2c_intr_status = READ_PERI_REG(I2C_I2C_INT_STATUS_REG(i2c_no));

	uint16 fifo_len = I2C_GET_RXFIFO_CNT(i2c_no);
	uint16 tx_fifo_len = I2C_GET_TXFIFO_CNT(i2c_no);
    uint16 i = 0;
os_printf("====================================================");
os_printf("IN I2C RX INTR !!!!!!!!\r\n");
os_printf("\n\n\n\n===================================================");

	os_printf("i2c_intr_status: 0x%08x \r\n",i2c_intr_status);
    if( (i2c_intr_status >> I2C_RXFIFO_FULL_INT_ST_S)&I2C_RXFIFO_FULL_INT_ST_V){
        os_printf(I2C_INT_STR"I2C_RXFIFO_FULL_INT, fifo_len: %d\r\n",fifo_len);
		for(i=0;i<fifo_len;i++){
            os_printf("%c",I2C_GET_FIFO_RDATA(i2c_no));
		}
    }
	else if( (i2c_intr_status >> I2C_TXFIFO_EMPTY_INT_ST_S)&I2C_TXFIFO_EMPTY_INT_ST_V){
        os_printf(I2C_INT_STR"I2C_TXFIFO_EMPTY_INT, TX_fifo_len: %d\r\n",tx_fifo_len);
        I2C_SET_TXFIFO_EMPTY_INT_ENA(i2c_no,0);
    }
	else if( (i2c_intr_status >> I2C_RXFIFO_OVF_INT_ST_S)&I2C_RXFIFO_OVF_INT_ST_V){
        os_printf(I2C_INT_STR"I2C_RXFIFO_OVF_INT, fifo_len: %d\r\n",fifo_len);
		for(i=0;i<fifo_len;i++){
            os_printf("%c",I2C_GET_FIFO_RDATA(i2c_no));
		}
    }
	else if( (i2c_intr_status >> I2C_SLAVE_TRAN_COMP_INT_ST_S)&I2C_SLAVE_TRAN_COMP_INT_ST_V){
        os_printf(I2C_INT_STR"I2C_SLAVE_TRAN_COMP_INT_ST, fifo_len: %d\r\n",fifo_len);
    }
	else if( (i2c_intr_status >> I2C_ARBITRATION_LOST_INT_ST_S)&I2C_ARBITRATION_LOST_INT_ST_V){
        os_printf(I2C_INT_STR"I2C_ARBITRATION_LOST_INT_ST, fifo_len: %d\r\n",fifo_len);
    }
	else if( (i2c_intr_status >> I2C_MASTER_TRAN_COMP_INT_ST_S)&I2C_MASTER_TRAN_COMP_INT_ST_V){
        os_printf(I2C_INT_STR"I2C_MASTER_TRAN_COMP_INT_ST, fifo_len: %d\r\n",fifo_len);
    }
	else if( (i2c_intr_status >> I2C_TRANS_COMPLETE_INT_ST_S)&I2C_TRANS_COMPLETE_INT_ST_V){
        os_printf(I2C_INT_STR"I2C_TRANS_COMPLETE_INT_ST, fifo_len: %d\r\n",fifo_len);
    }
	else if( (i2c_intr_status >> I2C_TIME_OUT_INT_ST_S)&I2C_TIME_OUT_INT_ST_V){
        os_printf(I2C_INT_STR"I2C_TIME_OUT_INT_ST, fifo_len: %d\r\n",fifo_len);
		os_printf("DISABLE TOUT INT...time out status: %d \r\n",I2C_GET_TIME_OUT(0));
		I2C_SET_TIME_OUT_INT_ENA(i2c_no,0);
    }
	else if( (i2c_intr_status >> I2C_TRANS_START_INT_ST_S)&I2C_TRANS_START_INT_ST_V){
        os_printf(I2C_INT_STR"I2C_TRANS_START_INT_ST, fifo_len: %d\r\n",fifo_len);
    }

	//uart_intr_status = READ_PERI_REG(UART_INT_ST_REG(uart_no)) ;
    SET_PERI_REG_MASK(I2C_I2C_INT_CLR_REG(i2c_no),i2c_intr_status);


}

#define I2C1_INT_STR  "I2C0 INTR:"

void i2c1_rx_intr_handler()
{
    uint8 i2c_no = 1;  //UART1
    uint32 i2c_intr_status = READ_PERI_REG(I2C_I2C_INT_STATUS_REG(i2c_no));

	uint16 fifo_len = I2C_GET_RXFIFO_CNT(i2c_no);
	uint16 tx_fifo_len = I2C_GET_TXFIFO_CNT(i2c_no);
    uint16 i = 0;
os_printf("====================================================");
os_printf("IN I2C RX INTR !!!!!!!!\r\n");
os_printf("\n\n\n\n===================================================");

	os_printf("i2c_intr_status: 0x%08x \r\n",i2c_intr_status);
    if( (i2c_intr_status >> I2C_RXFIFO_FULL_INT_ST_S)&I2C_RXFIFO_FULL_INT_ST_V){
        os_printf(I2C1_INT_STR"I2C_RXFIFO_FULL_INT, fifo_len: %d\r\n",fifo_len);
		for(i=0;i<fifo_len;i++){
            os_printf("%c",I2C_GET_FIFO_RDATA(i2c_no));
		}
    }
	else if( (i2c_intr_status >> I2C_TXFIFO_EMPTY_INT_ST_S)&I2C_TXFIFO_EMPTY_INT_ST_V){
        os_printf(I2C1_INT_STR"I2C_TXFIFO_EMPTY_INT, TX_fifo_len: %d\r\n",tx_fifo_len);
        I2C_SET_TXFIFO_EMPTY_INT_ENA(i2c_no,0);
    }
	else if( (i2c_intr_status >> I2C_RXFIFO_OVF_INT_ST_S)&I2C_RXFIFO_OVF_INT_ST_V){
        os_printf(I2C1_INT_STR"I2C_RXFIFO_OVF_INT, fifo_len: %d\r\n",fifo_len);
		for(i=0;i<fifo_len;i++){
            os_printf("%c",I2C_GET_FIFO_RDATA(i2c_no));
		}
    }
	else if( (i2c_intr_status >> I2C_SLAVE_TRAN_COMP_INT_ST_S)&I2C_SLAVE_TRAN_COMP_INT_ST_V){
        os_printf(I2C1_INT_STR"I2C_SLAVE_TRAN_COMP_INT_ST, fifo_len: %d\r\n",fifo_len);
    }
	else if( (i2c_intr_status >> I2C_ARBITRATION_LOST_INT_ST_S)&I2C_ARBITRATION_LOST_INT_ST_V){
        os_printf(I2C1_INT_STR"I2C_ARBITRATION_LOST_INT_ST, fifo_len: %d\r\n",fifo_len);
    }
	else if( (i2c_intr_status >> I2C_MASTER_TRAN_COMP_INT_ST_S)&I2C_MASTER_TRAN_COMP_INT_ST_V){
        os_printf(I2C1_INT_STR"I2C_MASTER_TRAN_COMP_INT_ST, fifo_len: %d\r\n",fifo_len);
    }
	else if( (i2c_intr_status >> I2C_TRANS_COMPLETE_INT_ST_S)&I2C_TRANS_COMPLETE_INT_ST_V){
        os_printf(I2C1_INT_STR"I2C_TRANS_COMPLETE_INT_ST, fifo_len: %d\r\n",fifo_len);
    }
	else if( (i2c_intr_status >> I2C_TIME_OUT_INT_ST_S)&I2C_TIME_OUT_INT_ST_V){
        os_printf(I2C1_INT_STR"I2C_TIME_OUT_INT_ST, fifo_len: %d\r\n",fifo_len);
		os_printf("DISABLE TOUT INT...time out status: %d \r\n",I2C_GET_TIME_OUT(0));
		I2C_SET_TIME_OUT_INT_ENA(i2c_no,0);
    }
	else if( (i2c_intr_status >> I2C_TRANS_START_INT_ST_S)&I2C_TRANS_START_INT_ST_V){
        os_printf(I2C1_INT_STR"I2C_TRANS_START_INT_ST, fifo_len: %d\r\n",fifo_len);
    }

	//uart_intr_status = READ_PERI_REG(UART_INT_ST_REG(uart_no)) ;
    SET_PERI_REG_MASK(I2C_I2C_INT_CLR_REG(i2c_no),i2c_intr_status);


}




void i2c_intr_config()
{
    os_printf("===========================\r\n");
	os_printf("I2C0 TEST INTR CONFIG\r\n");
    os_printf("===========================\r\n");
	
    ETS_INTR_DISABLE(ETS_I2C0_INUM);
	//REG_SET_FIELD(PRO_UART1_INTR_MAP_REG,DPORT_PRO_UART1_INTR_MAP,ETS_UART1_INUM);
    intr_matrix_set(ETS_I2C0_SOURCE, ETS_I2C0_INUM);
    ETS_INTR_ATTACH(i2c_rx_intr_handler, NULL,ETS_I2C0_INUM);
    ETS_INTR_ENABLE(ETS_I2C0_INUM);


}
#endif
#endif
#endif
