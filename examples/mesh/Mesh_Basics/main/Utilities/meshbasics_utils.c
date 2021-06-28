/*************************************************************************************************************
 * Copyright © 2018 Riccardo Bertini <m_bertini@hotmail.com>                                                 *
 * Licensed under the Apache License, Version 2.0 (the "License");                                           *
 * you may not use this file except in compliance with the License.                                          *
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0                        *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is      *
 * distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 * See the License for the specific language governing permissions and limitations under the License.        *
 *************************************************************************************************************/

/*************************************************************************
 *                               Headers
 *************************************************************************/
#include "meshbasics.h"                           //Main project library

/*************************************************************************
 *                        Functions Definitions
 *************************************************************************/

/* Converts a MAC address from string to uint8_t[6]
   (used in the Mesh Initializer Function, the MESH_EVENT_STARTED and the MESH_EVENT_SCAN_DONE events specific handlers, and in the Sender Driver) */
esp_err_t strToMAC(char* str, uint8_t* dest)
{
 int buf[6];                                      //Used as an intermediate buffer in the conversion

 if(sscanf(str,"%x:%x:%x:%x:%x:%x%*c",&buf[0],&buf[1],&buf[2],&buf[3],&buf[4],&buf[5]) == 6)  //Checks the MAC address format and converts it into an int[6] buffer
  {
   for(unsigned int i=0;i<6;i++)                  //Convert the MAC address from int[] to uint8_t[]
    dest[i] = (uint8_t)buf[i];
   return ESP_OK;
  }
 else                                             //If the MAC address format is invalid
  {
   printf("strToMAC(): Invalid MAC address format");
   return ESP_ERR_INVALID_ARG;
  }
}

/* Converts a SoftAP's authmode to string 
   (Used in the Mesh Initialzer Function and in the Logging Driver) */
void authToStr(wifi_auth_mode_t auth, char* dest)
{
 switch(auth)
  {
   case WIFI_AUTH_OPEN:
    strcpy(dest,"Open");
    break;
   case WIFI_AUTH_WPA_PSK:
    strcpy(dest,"WPA-PSK");
    break;
   case WIFI_AUTH_WPA2_PSK:
    strcpy(dest,"WPA2-PSK");
    break;
   case WIFI_AUTH_WPA_WPA2_PSK:
    strcpy(dest,"WPA/WPA2-PSK");
    break;
   default:
    strcpy(dest,"UNKNOWN");
    break;
  }
 return;
}

/* Converts a node's type to string
   (used in the MESH_EVENT_SCAN_DONE event specific handler and in the Logging Driver) */
void typeToStr(mesh_type_t type, char* dest)
{
 switch(type)
  {
   case MESH_IDLE:
    strcpy(dest,"IDLE");
    break;
   case MESH_ROOT:
    strcpy(dest,"ROOT");
    break;
   case MESH_NODE:
    strcpy(dest,"INTERMEDIATE_PARENT");
    break;
   case MESH_LEAF:
    strcpy(dest,"LEAF");
    break;
   default:
    strcpy(dest,"UNKNOWN");
    break;
  }
 return;
}