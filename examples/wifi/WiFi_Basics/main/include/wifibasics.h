/*************************************************************************************************************
 * Copyright © 2018 Riccardo Bertini <m_bertini@hotmail.com>                                                 *
 * Licensed under the Apache License, Version 2.0 (the "License");                                           *
 * you may not use this file except in compliance with the License.                                          *
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0                        *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is      *
 * distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 * See the License for the specific language governing permissions and limitations under the License.        *
 *************************************************************************************************************/

#ifndef WIFIBASICS_H
#define WIFIBASICS_H

/*************************************************************************
 *                               Headers
 *************************************************************************/

/*-- C Standard libraries --*/
#include <string.h>          	        //C standard string library

/*-- Environment-specific libraries --*/
#include "esp_system.h"         	    //ESP32 base system library
#include "esp_log.h"                	//ESP32 logging library
#include "nvs_flash.h"              	//ESP32 flash memory library
#include "esp_wifi.h"               	//ESP32 main Wi-Fi library
#include "esp_event_loop.h"         	//ESP32 Wi-Fi events Library
#include "freertos/FreeRTOS.h"      	//FreeRTOS base library
#include "freertos/task.h"          	//FreeRTOS tasks library
#include "freertos/event_groups.h"  	//FreeRTOS event groups library
#include "lwip/sockets.h"           	//LwIP base library

/*************************************************************************
 *                       Functions Declarations
 *************************************************************************/

/*-- Wi-Fi Initializer Function --*/
esp_err_t wifi_init();

/*-- Driver Components starting Functions --*/
void startStationNoIPDrivers();                   //Starts the StationNOIP driver components, i.e. the driver components that don't require the station interface to have an IP configuration set
void startStationIPDrivers();                     //Starts the StationIP driver components, i.e. the driver components that require the station interface to have an IP configuration set
void startSoftAPDrivers();                        //Starts the SoftAP driver components, i.e. the driver components relative to the SoftAP interface

/*-- Utility Functions --*/
esp_err_t strToMAC(char* str, uint8_t* dest);                   //Converts a MAC address from string to uint8_t[6]
void psToStr(wifi_ps_type_t ps_mode, char* dest);               //Converts the Station interface power saving mode to string
void authToStr(wifi_auth_mode_t auth, char* dest);              //Converts the SoftAP authmode to string 
void staCfgToStr(wifi_sta_config_t* sta_cfg, char* dest);       //Converts the parameters in the Station interface base configuration to string
void softapCfgToStr(wifi_ap_config_t* softap_cfg, char* dest);  //Converts the parameters in the SoftAP interface base configuration to string
void staDiscToStr(unsigned int reason, char* dest);             //Converts the reason the Station interface disconnected from its AP to string
bool parseApInfo(wifi_ap_record_t* ap, wifi_sta_config_t* sta_config, char* dest); //Converts the parameters relative to an AP found in a Wi-Fi scan to string and 
                                                                                   //checks whether it represents the target AP the Station interface should connect to

/*************************************************************************
 *                    Global Variables Declarations                            
 *************************************************************************/
extern EventGroupHandle_t wifi_event_group;       //Wi-Fi Event Group handler

/*-- Station interface Flags --*/
#define STA_ON                            BIT0    //Whether the Station interface is enabled or not
#define STA_CONN                          BIT1    //Whether the Station interface is connected to an AP or not
#define STA_GOTIP                         BIT2    //Whether the Station interface has an IP configuration set or not

/*-- SoftAP interface Flags --*/
#define SOFTAP_ON                         BIT3    //Whether the SoftAP interface is enabled or not
#define SOFTAP_CLI                        BIT4    //Whether the SoftAP interface has clients connected or not

/*-- Driver Components Executions Flags --*/
#define WIFI_DRIVER_STATION_NOIP_LOGGING  BIT5    //Whether the StationNOIP Logging Driver is running or not
#define WIFI_DRIVER_STATION_IP_LOGGING    BIT6    //Whether the StationIP Logging Driver is running or not
#define WIFI_DRIVER_SOFTAP_LOGGING        BIT7    //Whether the SoftAP Logging Driver is running or not


#endif  //WIFIBASICS_H