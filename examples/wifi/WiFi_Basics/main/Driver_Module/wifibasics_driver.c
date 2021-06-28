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
#include "wifibasics.h"         	        //Main project library
#include "wifibasics_driver_menuconfig.h"   //Driver Module menuconfig parameters

/*************************************************************************
 *     StationNOIP Logging Driver (WIFI_DRIVER_STATION_NOIP_LOGGING)
 *************************************************************************/
void stationNOIPLoggingDriver()
{
 const static char* TAG = "[StationNOIP Logging Driver]";
 EventBits_t eventbits;                                //Used to check the flags in the Wi-Fi event group

 uint8_t mac[6];                                       //Used to store the MAC address of the Station interface
 wifi_ps_type_t sta_ps_mode;                           //Used to store the Station interface power saving mode
 wifi_ap_record_t sta_ap;                              //Used to store information on the AP the Station interface is connected to
 char sta_ps_str[8];                                   //Used to print the Station power saving mode as a string
 char ap_auth_str[14];                                 //Used to print the authmode of the AP the Station interface is connected to as a string

 vTaskDelay(2000/portTICK_PERIOD_MS);                  //Initial delay (for a cleaner output)
 
 //Logging loop (that runs while the Station interface is connected to an AP)
 while(((eventbits = xEventGroupGetBits(wifi_event_group))>>1)&1)
  {
   ESP_ERROR_CHECK(esp_wifi_get_mac(ESP_IF_WIFI_STA,mac));             //Retrieve the MAC address of the Station interface
   ESP_ERROR_CHECK(esp_wifi_get_ps(&sta_ps_mode));                     //Retrieve the Station interface power saving mode
   ESP_ERROR_CHECK(esp_wifi_sta_get_ap_info(&sta_ap));                 //Retrieve information on the AP the Station interface is connected to  
   psToStr(sta_ps_mode,sta_ps_str);                                    //Convert the Station interface power saving mode to string
   authToStr(sta_ap.authmode,ap_auth_str);                             //Convert the SoftAP authmode to string

   ESP_LOGI(TAG,"======================== [StationNOIP Logging Driver] ========================");
   ESP_LOGI(TAG,"Station MAC Address = "MACSTR", Station Power Saving Mode = %s",MAC2STR(mac),sta_ps_str);
   ESP_LOGI(TAG,"");
   ESP_LOGI(TAG,"Connected AP information:");
   ESP_LOGI(TAG,"------------------------------------------------------------------------------");
   ESP_LOGI(TAG,"SSID = %s, BSSID = "MACSTR", Authmode = %s",sta_ap.ssid,MAC2STR(sta_ap.bssid),ap_auth_str);
   ESP_LOGI(TAG,"Wi-Fi Channel = %u, RSSI = %idb",sta_ap.primary,sta_ap.rssi);                    
   ESP_LOGI(TAG,"==============================================================================\n");
   vTaskDelay(WIFI_DRIVER_STATION_NOIP_LOGGING_INTERVAL*1000/portTICK_PERIOD_MS);
  }
 ESP_LOGE(TAG,"The Station interface is no longer connected to an AP, StationNOIP Logging Driver shutting down\n");
 xEventGroupClearBits(wifi_event_group,WIFI_DRIVER_STATION_NOIP_LOGGING);
 vTaskDelete(NULL);
}

/*************************************************************************
 *       StationIP Logging Driver (WIFI_DRIVER_STATION_IP_LOGGING)
 *************************************************************************/
void stationIPLoggingDriver()
{
 const static char* TAG = "[StationIP Logging Driver]";
 EventBits_t eventbits;                                //Used to check the flags in the Wi-Fi event group

 tcpip_adapter_ip_info_t ipinfo;                       //Used to store the Station interface IP configuration
 tcpip_adapter_dns_info_t dns_primary;                 //Used to store the address of the Station interface's primary DNS server
 tcpip_adapter_dns_info_t dns_secondary;               //Used to store the address of the Station interface's primary DNS server

 vTaskDelay(2500/portTICK_PERIOD_MS);                  //Initial delay (for a cleaner output)
 eventbits = xEventGroupGetBits(wifi_event_group);     //Retrieve the value of the Wi-Fi Event Group
 
 //Logging loop (that runs while the Station interface is connected to an AP AND has an IP configuration set)
 while(((eventbits>>1)&1)&&((eventbits>>2)&1))
  {
   ESP_ERROR_CHECK(tcpip_adapter_get_ip_info(ESP_IF_WIFI_STA,&ipinfo));                                  //Retrieve the Station interface IP configuration
   ESP_ERROR_CHECK(tcpip_adapter_get_dns_info(ESP_IF_WIFI_STA,TCPIP_ADAPTER_DNS_MAIN,&dns_primary));     //Retrieve the address of the Station interface's primary DNS server
   ESP_ERROR_CHECK(tcpip_adapter_get_dns_info(ESP_IF_WIFI_STA,TCPIP_ADAPTER_DNS_BACKUP,&dns_secondary)); //Retrieve the address of the Station interface's primary DNS server
                 
   ESP_LOGI(TAG,"========================= [StationIP Logging Driver] =========================");
   ESP_LOGI(TAG,"Station interface IP Configuration:");
   ESP_LOGI(TAG,"------------------------------------------------------------------------------");
   ESP_LOGI(TAG,"IP = "IPSTR", NetMask = "IPSTR", Gateway = "IPSTR,IP2STR(&ipinfo.ip),IP2STR(&ipinfo.netmask),IP2STR(&ipinfo.gw));
   ESP_LOGI(TAG,"Primary DNS = "IPSTR", Secondary DNS = "IPSTR,IP2STR(&dns_primary.ip.u_addr.ip4),IP2STR(&dns_secondary.ip.u_addr.ip4));
   ESP_LOGI(TAG,"==============================================================================\n");
   vTaskDelay(WIFI_DRIVER_STATION_IP_LOGGING_INTERVAL*1000/portTICK_PERIOD_MS);
   eventbits = xEventGroupGetBits(wifi_event_group);   //Retrieve the value of the Wi-Fi Event Group
  }
 if(!((eventbits>>1)&1))                               //If the Station interface is no longer connected to an AP
  ESP_LOGE(TAG,"The Station interface is no longer connected to an AP, StationIP Logging Driver shutting down\n");
 else                                                  //If the Station interface lost its IP configuration
  ESP_LOGE(TAG,"The Station interface has no longer an IP configuration set, StationIP Logging Driver shutting down\n");
 xEventGroupClearBits(wifi_event_group,WIFI_DRIVER_STATION_IP_LOGGING);
 vTaskDelete(NULL);
}

/*************************************************************************
 *          SoftAP Logging Driver (WIFI_DRIVER_SOFTAP_LOGGING)
 *************************************************************************/
void softapLoggingDriver()
{
 const static char* TAG = "[SoftAP Logging Driver]";
 EventBits_t eventbits;                                //Used to check the flags in the Wi-Fi event group

 uint8_t mac[6];                                       //Used to store the MAC address of the SoftAP interface
 wifi_config_t softap_config;                          //Used to store the SoftAP interface base configuration
 tcpip_adapter_ip_info_t ipinfo;                       //Used to store the SoftAP interface IP configuration
 tcpip_adapter_dns_info_t dns_primary;                 //Used to store the address of the SoftAP interface's primary DNS server
 wifi_sta_list_t clients;                              //Used to store information on the clients connected to the SoftAP interface
 char softap_config_str[200];                          //Used to print the parameters of the SoftAP Base Configuration as a string

 vTaskDelay(2000/portTICK_PERIOD_MS);                  //Initial delay (for a cleaner output)

 //Logging loop (that runs while the SoftAP interface is enabled)
 while(((eventbits = xEventGroupGetBits(wifi_event_group))>>3)&1)
  {
   ESP_ERROR_CHECK(esp_wifi_get_mac(ESP_IF_WIFI_AP,mac));                                                //Retrieve the MAC address of the SoftAP interface
   ESP_ERROR_CHECK(esp_wifi_get_config(ESP_IF_WIFI_AP,&softap_config));                                  //Retrieve the SoftAP interface base configuration
   ESP_ERROR_CHECK(tcpip_adapter_get_ip_info(ESP_IF_WIFI_AP,&ipinfo));                                   //Retrieve the SoftAP interface IP configuration
   ESP_ERROR_CHECK(tcpip_adapter_get_dns_info(ESP_IF_WIFI_AP,TCPIP_ADAPTER_DNS_MAIN,&dns_primary));      //Retrieve the address of the Station interface's primary DNS server
   softapCfgToStr(&softap_config.ap,softap_config_str);                                                  //Convert the parameters in the SoftAP interface base configuration to string

   ESP_LOGI(TAG,"=========================== [SoftAP Logging Driver] ===========================");
   ESP_LOGI(TAG,"%s",softap_config_str);
   ESP_LOGI(TAG,"IP = "IPSTR", NetMask = "IPSTR", Gateway = "IPSTR,IP2STR(&ipinfo.ip),IP2STR(&ipinfo.netmask),IP2STR(&ipinfo.gw));
   ESP_LOGI(TAG,"Primary DNS = "IPSTR", Secondary DNS = (none)",IP2STR(&dns_primary.ip.u_addr.ip4));     //Note that as of the current ESP-IDF version SoftAP 
                                                                                                         //interfaces don't support a secondary DNS server set
   if(WIFI_DRIVER_SOFTAP_LOGGING_CLIENTS)                                                                //Whether to print information on the clients connected to the SoftAP interface
    {
     ESP_ERROR_CHECK(esp_wifi_ap_get_sta_list(&clients));                                                //Retrieve information on the clients connected to the SoftAP interface
 
     ESP_LOGI(TAG,"");
     ESP_LOGI(TAG,"Connected Clients: %u (max: %u)",clients.num,softap_config.ap.max_connection);      
     if(clients.num>0)
      {
       ESP_LOGI(TAG,"-------------------------------------------------------------------------------");
       for(unsigned int i=0;i<clients.num;i++)
        ESP_LOGI(TAG,"%u) MAC = "MACSTR", RSSI = %i",i+1,MAC2STR(clients.sta[i].mac),clients.sta[i].rssi);
      }
    }
   ESP_LOGI(TAG,"===============================================================================\n");      
   vTaskDelay(WIFI_DRIVER_SOFTAP_LOGGING_INTERVAL*1000/portTICK_PERIOD_MS);
  }
 ESP_LOGE(TAG,"The SoftAP interface is no longer enabled, SoftAP Logging Driver shutting down\n");
 xEventGroupClearBits(wifi_event_group,WIFI_DRIVER_SOFTAP_LOGGING);
 vTaskDelete(NULL);
}

/*************************************************************************
 *                Driver Components Starting Functions
 *************************************************************************/

/* Starts the StationNOIP driver components, i.e. the driver components that don't require the station interface to have an IP configuration set
   (called at the end of the SYSTEM_EVENT_STA_CONNECTED event specific handler)                                                                  */
void startStationNoIPDrivers()
{
 const static char* TAG = "[startStationNOIPDrivers]";
 EventBits_t eventbits;                                   //Used to check the flags in the Wi-Fi event group
 static TaskHandle_t stationnoip_logging_handler;         //StationNOIP Logging Driver task handler

 /* For each StationNOIP driver component, if the related task is already running kill it before creating it again */
 if(WIFI_DRIVER_STATION_NOIP_LOGGING_ENABLE)                                     //If the StationNOIP Logging Driver should be executed on the device                                    
  {
   if(((eventbits = xEventGroupGetBits(wifi_event_group))>>5)&1)                 //If the StationNOIP Logging Driver task is already running (WIFI_DRIVER_STATION_NOIP_LOGGING)
    {
     ESP_LOGW(TAG,"-- Deleting the existing StationNOIP Logging Driver task");
     vTaskDelete(stationnoip_logging_handler);                                   //Kill the existing StationNOIP Logging Driver task
    }
   ESP_LOGW(TAG,"-- Starting the StationNOIP Logging Driver task");
   xEventGroupSetBits(wifi_event_group,WIFI_DRIVER_STATION_NOIP_LOGGING);        //Set the WIFI_DRIVER_STATION_NOIP_LOGGING flag in the Wi-Fi event group
   xTaskCreate(stationNOIPLoggingDriver,"wifistationnoiploggingdriver",4096,NULL,10,&stationnoip_logging_handler);  //Create (and run) the StationNOIP Logging Driver task
  }
 return;
}

/* Starts the StationIP driver components, i.e. the driver components that require the station interface to have an IP configuration set
   (called at the end of the SYSTEM_EVENT_STA_GOT_IP event specific handler)                                                             */
void startStationIPDrivers()
{
 const static char* TAG = "[startStationIPDrivers]";
 EventBits_t eventbits;                                   //Used to check the flags in the Wi-Fi event group
 static TaskHandle_t stationip_logging_handler;           //StationIP Logging Driver task handler

 /* For each StationIP driver component, if the related task is already running kill it before creating it again */
 if(WIFI_DRIVER_STATION_IP_LOGGING_ENABLE)                                       //If the StationIP Logging Driver should be executed on the device                                    
  {
   if(((eventbits = xEventGroupGetBits(wifi_event_group))>>6)&1)                 //If the StationIP Logging Driver task is already running (WIFI_DRIVER_STATION_IP_LOGGING)
    {
     ESP_LOGW(TAG,"-- Deleting the existing StationIP Logging Driver task");
     vTaskDelete(stationip_logging_handler);                                     //Kill the existing StationIP Logging Driver task
    }
   ESP_LOGW(TAG,"-- Starting the StationIP Logging Driver task");
   xEventGroupSetBits(wifi_event_group,WIFI_DRIVER_STATION_IP_LOGGING);          //Set the WIFI_DRIVER_STATION_IP_LOGGING flag in the Wi-Fi event group
   xTaskCreate(stationIPLoggingDriver,"wifistationiploggingdriver",4096,NULL,10,&stationip_logging_handler);   //Create (and run) the StationIP Logging Driver task
  }
 return;
}

/* Starts the SoftAP driver components, i.e. the driver components relative to the SoftAP interface
   (called at the end of the SYSTEM_EVENT_AP_START event specific handler)                          */
void startSoftAPDrivers()
{
 const static char* TAG = "[startSoftAPDrivers]";
 EventBits_t eventbits;                                   //Used to check the flags in the Wi-Fi event group
 static TaskHandle_t softap_logging_handler;              //SoftAP Logging Driver task handler

 /* For each SoftAP driver component, if the related task is already running kill it before creating it again */
 if(WIFI_DRIVER_SOFTAP_LOGGING_ENABLE)                                           //If the SoftAP Logging Driver should be executed on the device                                    
  {
   if(((eventbits = xEventGroupGetBits(wifi_event_group))>>7)&1)                 //If the SoftAP Logging Driver task is already running (WIFI_DRIVER_SOFTAP_LOGGING)
    {
     ESP_LOGW(TAG,"-- Deleting the existing SoftAP Logging Driver task");
     vTaskDelete(softap_logging_handler);                                        //Kill the existing SoftAP Logging Driver task
    }
   ESP_LOGW(TAG,"-- Starting the SoftAP Logging Driver task");
   xEventGroupSetBits(wifi_event_group,WIFI_DRIVER_SOFTAP_LOGGING);                      //Set the WIFI_DRIVER_SOFTAP_LOGGING flag in the Wi-Fi event group
   xTaskCreate(softapLoggingDriver,"softaploggingdriver",4096,NULL,10,&softap_logging_handler);         //Create (and run) the StationIP Logging Driver task
  }
 return;
}