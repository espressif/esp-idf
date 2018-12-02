/*************************************************************************************************************
 * Copyright © 2018 Riccardo Bertini <m_bertini@hotmail.com>                                                 *
 * Licensed under the Apache License, Version 2.0 (the "License");                                           *
 * you may not use this file except in compliance with the License.                                          *
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0                        *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is      *
 * distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 * See the License for the specific language governing permissions and limitations under the License.        *
 *************************************************************************************************************/

#ifndef MESHBASICS_H
#define MESHBASICS_H

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
#include "esp_mesh.h"                   //ESP32 main mesh Library
#include "freertos/FreeRTOS.h"      	//FreeRTOS base library
#include "freertos/task.h"          	//FreeRTOS tasks library
#include "freertos/event_groups.h"  	//FreeRTOS event groups library
#include "lwip/sockets.h"           	//LwIP base library

/*************************************************************************
 *                          Type Definitions
 *************************************************************************/
typedef enum                           //Mesh Organization Mode
{
 SELF_ORGANIZED,                       //Self-Organized Networking
 FIXED_ROOT,                           //Fixed-Root Networking
 MANUAL_NETWORKING                     //Manual Networking
} mesh_org_t;

typedef struct                         //Describes the status of a node in the mesh network
{
 uint8_t mid[6];                       //Mesh Network Identifier (MID)
 uint8_t channel;                      //Mesh Wi-Fi Channel
 mesh_org_t org;                       //Mesh Organization Mode
 uint8_t root_addr[6];                 //Root node's SoftAP MAC address
 uint8_t parent_addr[6];               //Node's parent SoftAP MAC address
 mesh_type_t my_type;                  //Node's type
 int8_t my_layer;                      //Node's layer in the mesh network
} mesh_status_t;

/*************************************************************************
 *                       Functions Declarations
 *************************************************************************/

/*-- Mesh Initializer Function --*/
esp_err_t mesh_init();

/*-- Driver Components starting Functions --*/
void startMeshInternalDrivers();       //Starts the internal driver components, i.e. the driver components that don't require the node to access the external DS
void startMeshExternalDrivers();       //Starts the external driver components, i.e. the driver components that require the node to access the external DS
void startMeshRootDrivers();           //Starts the root driver components, i.e. the driver components that must be executed on the root node of the mesh network only

/*-- Utility Functions --*/
esp_err_t strToMAC(char* str, uint8_t* dest);       //Converts a MAC address from string to uint8_t[6]
void authToStr(wifi_auth_mode_t auth, char* dest);  //Converts a SoftAP's authmode to string
void typeToStr(mesh_type_t type, char* dest);       //Converts a node's type to string

/*************************************************************************
 *                    Global Variables Declarations                            
 *************************************************************************/
extern EventGroupHandle_t mesh_event_group;     //Mesh Event Group handler

/*-- Mesh Setup Flags --*/
#define MESH_ON                        BIT0     //Whether mesh networking is enabled on the node or not
#define MESH_PARENT                    BIT1     //Whether the node is connected to a parent or not
#define MESH_CHILD                     BIT2     //Whether the node has children connected or not
#define MESH_TODS                      BIT3     //Whether the external DS is reachable or not
#define MESH_VOTE                      BIT4     //Whether a new root election is currently in progress in the mesh network or not (active low, 0 = election in progress, 1 = no election)

/*-- Driver Components Executions Flags --*/
#define MESH_DRIVER_INTERNAL_LOGGING   BIT5     //Whether the internal Logging Driver is running or not
#define MESH_DRIVER_INTERNAL_SENDER    BIT6     //Whether the internal Sender Driver is running or not
#define MESH_DRIVER_INTERNAL_RECEIVER  BIT7     //Whether the internal Receiver Driver is running or not
#define MESH_DRIVER_EXTERNAL_SENDERDS  BIT8     //Whether the external SenderDS Driver is running or not
#define MESH_DRIVER_ROOT_RECEIVERDS    BIT9     //Whether the root ReceiverDS Driver is running or not
#define MESH_DRIVER_ROOT_WAIVE         BIT10    //Whether the root Waive Driver is running or not

extern mesh_status_t mesh_state;                //Describes the node's current status in the mesh network


#endif  //MESHBASICS_H