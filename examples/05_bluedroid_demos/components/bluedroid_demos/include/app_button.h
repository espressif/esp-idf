/**
 ****************************************************************************************
 *
 * @file app_button.h
 *
 * @brief button Service Application entry point
 *
 * Copyright (C) ESPRESSIF 2016
 * Created by Yulong at 2016/02/24
 *
 *
 ****************************************************************************************
 */

 /*
 * DEFINES
 ****************************************************************************************
 */
#define BUTTON_HEAD		(0x01)
#define BUTTON_PRESS_EVT (0x10)

//the key value enum
enum 
{
	Button_Up = 0x01,
	Button_Voice = 0x02,
	Button_OK = 0x04,
	Button_Down = 0x08,
	Button_Back = 0x10,
};


typedef struct {
	uint8_t  key_val;	//button val
	uint8_t head;		//the head of the frame
}key_frame;


typedef struct 
{
	bool button_press;
	key_frame key_msg;
}app_key_env;



extern app_key_env key_press;

/*
 * FUNCTIONS DECLARATION
 ****************************************************************************************
 */

 void app_button_init(void);

 void GPIO_isr_callback(void* arg);

 uint8_t check_sum(uint8_t *check_array,uint8_t len);




