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
#include "wifibasics.h"                           //Main project library

/*************************************************************************
 *                        Functions Definitions
 *************************************************************************/

/*Converts a MAC address from string to uint8_t[6] 
   (used in the Wi-Fi Initializer Function)                 */
esp_err_t strToMAC(char* str, uint8_t* dest)
{
 int buf[6];                                //Used as an intermediate buffer in the conversion

 if(sscanf(str,"%x:%x:%x:%x:%x:%x%*c",&buf[0],&buf[1],&buf[2],&buf[3],&buf[4],&buf[5]) == 6)  //Checks the MAC address format and converts it into an int[6] buffer
  {
   for(unsigned int i=0;i<6;i++)            //Convert the MAC address from int[] to uint8_t[]
    dest[i] = (uint8_t)buf[i];
   return ESP_OK;
  }
 else                                       //If the MAC address format is invalid
  {
   printf("strToMAC(): Invalid MAC address format");
   return ESP_ERR_INVALID_ARG;
  }
}

/* Converts the Station interface power saving mode to string
   (used in the Wi-Fi Initializer Function and the StationNOIP Logging Driver) */
void psToStr(wifi_ps_type_t ps_mode, char* dest)
{
 switch(ps_mode)
  {
   case WIFI_PS_NONE:
    strcpy(dest,"None");
    break;
   case WIFI_PS_MIN_MODEM:
    strcpy(dest,"Minimum");
    break;
   case WIFI_PS_MAX_MODEM:
    strcpy(dest,"Maximum");
    break;
   default:
    strcpy(dest,"UNKNOWN");
    break;
  }
 return;
}

/* Converts the SoftAP's authmode to string 
   (used in the "softapCfgToStr" and "parseApInfo" utility functions and in the StationNOIP Logging Driver) */
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

/* Converts the parameters in the Station interface base configuration to string
   (used in the Wi-Fi Initializer Function and in the SYSTEM_EVENT_STA_START event specific handler) */
void staCfgToStr(wifi_sta_config_t* sta_cfg, char* dest)
{
 //Build a string from the settings of the Station base configuration 
 sprintf(dest,"SSID = %s",sta_cfg->ssid);                     
 if(strlen((char*)sta_cfg->password)>0)
  sprintf(dest + strlen(dest),", Password = %s",sta_cfg->password);
 else
  strcat(dest,", Password = (none)");

 //The BSSID is added only if the Station interface must connect to an AP with a specific BSSID 
 if(sta_cfg->bssid_set)     
  sprintf(dest + strlen(dest),", specific BSSID = "MACSTR,MAC2STR(sta_cfg->bssid));
 return;
}

/* Converts the parameters in the SoftAP interface base configuration to string
   (used in the Wi-Fi Initializer Function, the SYSTEM_EVENT_AP_START event specific handler and in the SoftAP Logging Driver) */
void softapCfgToStr(wifi_ap_config_t* softap_cfg, char* dest)
{
 char softap_auth_str[14];                                              //Used to print the SoftAP authmode as a string

 //Build a string from the settings of the SoftAP base configuration 
 sprintf(dest,"SSID = %s",softap_cfg->ssid);
 if(softap_cfg->ssid_hidden)
  strcat(dest," (hidden)"); 
 authToStr(softap_cfg->authmode,softap_auth_str);                       //Convert the SoftAP Authmode to string
 sprintf(dest + strlen(dest),", Authmode = %s",softap_auth_str);
 if(strlen((char*)softap_cfg->password)>0)
  sprintf(dest + strlen(dest),", Password = %s",softap_cfg->password);
 else
  strcat(dest,", Password = (none)");
 if(softap_cfg->channel)
  sprintf(dest + strlen(dest),", Wi-Fi Channel = %u",softap_cfg->channel);
 else
  strcat(dest,", Wi-Fi Channel = (auto)");
 sprintf(dest + strlen(dest),", Max Connections = %u",softap_cfg->max_connection);
 sprintf(dest + strlen(dest),", Beacon Interval = %ums",softap_cfg->beacon_interval);
 return;
}

/* Checks whether two arrays are equal
   (used in the "parseApInfo" utility function) */
bool checkArrayEqual(uint8_t* arr1, uint8_t* arr2, uint8_t size)
{
 for(unsigned int i=0;i<size;i++)
  if(arr1[i]!=arr2[i])
   return false;                  //The two arrays are NOT equal
 return true;                     //The two arrays are equal
}

/* Converts the parameters relative to an AP found in a Wi-Fi scan to string and checks whether it represents the target AP the Station interface should connect to
   (used in the SYSTEM_EVENT_STA_SCAN_DONE event specific handler)                                                                                                  */ 
bool parseApInfo(wifi_ap_record_t* ap, wifi_sta_config_t* sta_config, char* dest)
{
 char ap_auth_str[14];                                             //Used to print the scanned AP authmode as a string

 //Build a string from the AP's parameters
 if(strlen((char*)ap->ssid)==0)
  sprintf(dest,"SSID = (hidden)");                     
 else
  sprintf(dest,"SSID = %s",ap->ssid);                     
 sprintf(dest + strlen(dest),", BSSID = "MACSTR,MAC2STR(ap->bssid));
 authToStr(ap->authmode,ap_auth_str);                              //Convert the AP Authmode to string
 sprintf(dest + strlen(dest),", Authmode = %s",ap_auth_str);
 sprintf(dest + strlen(dest),", Wi-Fi Channel = %u",ap->primary);
 sprintf(dest + strlen(dest),", RSSI = %idb",ap->rssi);

 //Check whether the AP is the target AP the Station interface must connect to
 if(checkArrayEqual(ap->ssid,sta_config->ssid,32)&&((!sta_config->bssid_set)||(checkArrayEqual(ap->bssid,sta_config->bssid,6))))
  {
   sprintf(dest + strlen(dest)," (*)");
   return true;   //This is the target AP
  }
 if(ap->authmode == WIFI_AUTH_OPEN)
  sprintf(dest + strlen(dest)," (!)");
 return false;    //This is not the target AP
}

/* Converts the reason the Station interface disconnected from its AP to string
   (used in the SYSTEM_EVENT_STA_DISCONNECTED event specific handler)           */
void staDiscToStr(unsigned int reason, char* dest)
{ 
 dest[0] = '\0';                                      
 switch(reason)
  {
   case WIFI_REASON_AUTH_EXPIRE:
    strcpy(dest,"AUTH_EXPIRED (2)");
    break;
   case WIFI_REASON_AUTH_LEAVE:
    strcpy(dest,"AUTH_LEAVE (3)");
    break;
   case WIFI_REASON_ASSOC_EXPIRE:
    strcpy(dest,"ASSOC_EXPIRE (4)");
    break;
   case WIFI_REASON_ASSOC_TOOMANY:
    strcpy(dest,"ASSOC_TOOMANY (5)");
    break;
   case WIFI_REASON_NOT_AUTHED:
    strcpy(dest,"NOT_AUTHED (6)");
    break;   
   case WIFI_REASON_NOT_ASSOCED:
    strcpy(dest,"NOT_ASSOCIATED (7)");
    break;  
   case WIFI_REASON_ASSOC_LEAVE:
    strcpy(dest,"ASSOC_LEAVE (8)");
    break;  
   case WIFI_REASON_ASSOC_NOT_AUTHED:
    strcpy(dest,"ASSOC_NOT_AUTHED (9)");
    break;  
   case WIFI_REASON_DISASSOC_PWRCAP_BAD:
    strcpy(dest,"DISASSOC_PWRCAP_BAD (10)");
    break;  
   case WIFI_REASON_DISASSOC_SUPCHAN_BAD:
    strcpy(dest,"DISASSOC_SUPCHAN_BAD (11)");
    break;  
   case WIFI_REASON_IE_INVALID:
    strcpy(dest,"IE_INVALID (13)");
    break;  
   case WIFI_REASON_MIC_FAILURE:
    strcpy(dest,"MIC_FAILURE (14)");
    break;  
   case WIFI_REASON_4WAY_HANDSHAKE_TIMEOUT:
    strcpy(dest,"4WAY_HANDSHAKE_TIMEOUT (15)");
    break;  
   case WIFI_REASON_GROUP_KEY_UPDATE_TIMEOUT:
    strcpy(dest,"GROUP_KEY_UPDATE_TIMEOUT (16)");
    break;  
   case WIFI_REASON_IE_IN_4WAY_DIFFERS:
    strcpy(dest,"IE_IN_4WAY_DIFFERS (17)");
    break;  
   case WIFI_REASON_GROUP_CIPHER_INVALID:
    strcpy(dest,"GROUP_CIPHER_INVALID (18)");
    break;  
   case WIFI_REASON_PAIRWISE_CIPHER_INVALID:
    strcpy(dest,"PAIRWISE_CIPHER_INVALID (19)");
    break;  
   case WIFI_REASON_AKMP_INVALID:
    strcpy(dest,"AKMP_INVALID (20)");
    break;  
   case WIFI_REASON_UNSUPP_RSN_IE_VERSION:
    strcpy(dest,"UNSUPP_RSN_IE_VERSION (21)");
    break;  
   case WIFI_REASON_INVALID_RSN_IE_CAP:
    strcpy(dest,"INVALID_RSN_IE_CAP (22)");
    break;  
   case WIFI_REASON_802_1X_AUTH_FAILED:
    strcpy(dest,"802_1X_AUTH_FAILED (23)");
    break;  
   case WIFI_REASON_CIPHER_SUITE_REJECTED:
    strcpy(dest,"CIPHER_SUITE_REJECTED (24)");
    break;  

   case WIFI_REASON_BEACON_TIMEOUT:
    strcpy(dest,"BEACON_TIMEOUT (200)");
    break;  
   case WIFI_REASON_NO_AP_FOUND:
    strcpy(dest,"NO_AP_FOUND (201)");
    break;  
   case WIFI_REASON_AUTH_FAIL:
    strcpy(dest,"AUTH_FAIL (202)");
    break;  
   case WIFI_REASON_ASSOC_FAIL:
    strcpy(dest,"ASSOC_FAIL (203)");
    break;  
   case WIFI_REASON_HANDSHAKE_TIMEOUT:
    strcpy(dest,"HANDSHAKE_TIMEOUT (204)");
    break;  

   case WIFI_REASON_UNSPECIFIED:
    strcpy(dest,"UNKNOWN (1)");
    break;
   
   default:
    sprintf(dest,"UNKNOWN (%u)",reason);
    break;
  }
 return;
}