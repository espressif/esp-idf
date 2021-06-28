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
#include "wifibasics_setup_menuconfig.h"    //Setup Module menuconfig parameters

/*************************************************************************
 *                           Global Variables                  
 *************************************************************************/
EventGroupHandle_t wifi_event_group;        //Wi-Fi Event Group handler

/*************************************************************************
 *                    Wi-Fi Events Specific Handlers
 *************************************************************************/

/**===================== Station Interface Events =====================**/

/* Raised after the Station interface has been enabled on the device */
void wifi_STA_START_handler()
{
 static const char* TAG = "[STA_START handler]";
 wifi_scan_config_t scan_config = {0};              //Used to store the configuration of the preliminary Wi-Fi scan, should it be performed
 wifi_config_t sta_config;                          //Used to store the Station interface base configuration
 char sta_config_str[150];                          //Used to print the parameters of the Station Base configuration as a string

 xEventGroupSetBits(wifi_event_group,STA_ON);       //Set the STA_ON flag in the Wi-Fi Event Group
 ESP_LOGW(TAG,"Station interface enabled"); 
 if(WIFI_STATION_SCAN_PRELIMINARY)                  //If performing a preliminary Wi-Fi scan before attempting to connect the Station interface to its target AP 
  {
   ESP_LOGW(TAG,"Performing a preliminary Wi-Fi scan before attempting to connect to the target AP");
   if(WIFI_STATION_SCAN_HIDDEN)                     //If APs with hidden SSIDs should be included in the Wi-Fi scan results
    scan_config.show_hidden = 1;
   ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config,false)); //Perform an active all-channel Wi-Fi scan
  }
 else                                               //If the Station interface should attempt to directly to connect to its target AP
  {
   ESP_ERROR_CHECK(esp_wifi_get_config(ESP_IF_WIFI_STA,&sta_config)); //Retrieve the Station interface Base Configuration
   staCfgToStr(&sta_config.sta,sta_config_str);                       //Convert the parameters in the Station base configuration to string
   ESP_LOGW(TAG,"Attempting to directly connect to target AP with %s\n",sta_config_str);
   ESP_ERROR_CHECK(esp_wifi_connect());             //Attempt to connect to the target AP
  }
 return;
}

/* Raised after the Station interface became disabled, which can occur either intentionally by calling 
   the esp_wifi_stop() function (not used in this example) or due to a fatal error in the Wi-Fi stack  */
void wifi_STA_STOP_handler()
{
 static const char* TAG = "[STA_STOP handler]";
  
 xEventGroupClearBits(wifi_event_group,STA_ON);                               //Clear the STA_ON flag in the Wi-Fi Event group  
 ESP_LOGW(TAG,"Station interface became disabled, trying to reenable...\n");  //Since in this example the disabling of the Station interface is always unintentional, as an
 ESP_ERROR_CHECK(esp_wifi_start());                                           //error recovery attempt we to try to reenable it by calling the esp_wifi_start() function
 return;
}

/* Raised after the Wi-Fi stack completes a Wi-Fi scan requested by the application */
void wifi_SCAN_DONE_handler(system_event_sta_scan_done_t* info)
{
 static const char* TAG = "[SCAN_DONE handler]";
 wifi_scan_config_t scan_config = {0};    //Configuration of a new Wi-Fi scan to be performed if this scan reported an error or no suitable AP to connect to was found
 wifi_ap_record_t* ap_list = 0;           //Used to dynamically store the records of the APs found in the scan
 wifi_config_t sta_config;                //Used to store the Station interface base configuration
 uint16_t ap_num = info->number;          //Number of APs found in the Wi-Fi scan
 int target_ap = -1;                      //Whether the target AP has been found and its index in the AP list
 int open_ap = -1;                        //Whether an open AP to possibly fall back on has been found and its index in the AP list
 char buf[150];                           //Used to print the information relative to each AP found
 
 if((!info->status)&&ap_num)           //If the Wi-Fi scan was successful and at least one AP was found in the scan                  
  {                              
   ESP_ERROR_CHECK(esp_wifi_get_config(ESP_IF_WIFI_STA,&sta_config));    //Retrieve the Station interface base configuration
   ap_list = (wifi_ap_record_t*)malloc(ap_num*sizeof(wifi_ap_record_t)); //Dynamically allocate the memory required to store the records of the APs that were found in the scan
   ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&ap_num,ap_list));       //Retrieve the records of the APs that were found in the scan
   ESP_LOGW(TAG,"The following %u APs were found in the Wi-Fi Scan with ID = %u",ap_num,info->scan_id);
   ESP_LOGW(TAG,"-----------------------------------------------------------------------------------------------------------");
   for(unsigned int i=0;i<ap_num;i++)                                                                                       //Parse the APs records
    {
     if((parseApInfo((ap_list+i),&sta_config.sta,buf)) && ((target_ap<0)||((ap_list+i)->rssi>(ap_list+target_ap)->rssi)))   //Convert the information relative to each AP to string and check
       target_ap = i;                                                                                                       //whether it is the target AP the Station interface should connect to
     if((strlen((char*)(ap_list+i)->ssid)>0) && ((ap_list+i)->authmode == WIFI_AUTH_OPEN) && ((open_ap<0)||((ap_list+i)->rssi>(ap_list+open_ap)->rssi))) //Store the index of the open AP with the highest RSSI with the Station
      open_ap = i;                                                                                                                                       //interface to allow for a possible fallback if the target AP is not found
     ESP_LOGW(TAG,"%u) %s",i+1,buf);                                                                                        //Print the information relative to each AP
    }
   ESP_LOGW(TAG,"-----------------------------------------------------------------------------------------------------------");
    
   //Perform actions based on the APs that were found in the scan
   if((target_ap>=0)||(WIFI_STATION_SCAN_ALLOW_FALLBACK&&(open_ap>=0)))  //If the target AP or an open AP to fall back on was found
    {
     if(target_ap>=0)   //If the target AP was found
      ESP_LOGW(TAG,"The target AP was found in the Wi-Fi scan (%u), attempting to connect to it\n",target_ap+1);
     else               //If an open AP to fall back on was found
      {
       //Update the Station base configuration with the open AP's settings
       strcpy((char*)sta_config.sta.ssid,(char*)(ap_list+open_ap)->ssid);  //Set the open AP SSID
       sta_config.sta.password[0] = '\0';                                  //Set the open AP Password
       memcpy(sta_config.sta.bssid,(ap_list+open_ap)->bssid,6);            //Set the open AP BSSID
       sta_config.sta.bssid_set = 1;                                       //Connect to the AP with the specific BSSID
       sta_config.sta.channel = (ap_list+open_ap)->primary;                //Set the open AP Wi-Fi channel
      
       ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA,&sta_config));  //Update the Station interface base configuration with the open AP's settings
       ESP_LOGW(TAG,"The target AP was not found in the Wi-Fi scan, but an open AP is (%u), attempting to connect to it\n",open_ap+1); 
      }
     ESP_ERROR_CHECK(esp_wifi_connect());   //Attempt to connect the Station interface to the target or the fallback AP
    }
   else
    {
     if(WIFI_STATION_SCAN_ALLOW_FALLBACK)   //If the target AP or any open AP was not found 
      ESP_LOGW(TAG,"The target AP or any open AP was not found in the scan, performing a new scan in %i seconds\n",WIFI_STATION_SCAN_RETRY_INTERVAL);
     else                                   //If the fallback on an open AP is not allowed
      ESP_LOGW(TAG,"The target AP was not found in the scan, performing a new scan in %i seconds\n",WIFI_STATION_SCAN_RETRY_INTERVAL);
     if(WIFI_STATION_SCAN_HIDDEN)           //If APs with hidden SSIDs should be included in the next scan results
      scan_config.show_hidden = 1;
     vTaskDelay(WIFI_STATION_SCAN_RETRY_INTERVAL*1000/portTICK_PERIOD_MS);
     ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config,false));  //Perform a new active all-channel Wi-Fi scan
    }
   free(ap_list);                           //Release the dynamic memory used to store the information on the APs found in the scan
  }
 else  //Otherwise, if an error occured in the Wi-Fi scan or no APs were found, perform another Wi-Fi scan after a set time interval
  {
   if(info->status)
    ESP_LOGW(TAG,"The Wi-Fi scan with ID = %u returned the error \"%u\"",info->scan_id,info->status);
   else
    ESP_LOGW(TAG,"The Wi-Fi scan with ID = %u completed successfully, but no APs were found",info->scan_id);
   ESP_LOGW(TAG,"Performing a new Wi-Fi Scan in %i seconds...\n",WIFI_STATION_SCAN_RETRY_INTERVAL);
   if(WIFI_STATION_SCAN_HIDDEN)             //If APs with hidden SSIDs should be included in the Wi-Fi scan results
    scan_config.show_hidden = 1;
   vTaskDelay(WIFI_STATION_SCAN_RETRY_INTERVAL*1000/portTICK_PERIOD_MS);
   ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config,false));    //Perform an all-channel Wi-Fi scan
  }
 return;
}

/* Raised after the Station interface successfully connects to an AP */
void wifi_STA_CONNECTED_handler(system_event_sta_connected_t* info)
{
 static const char* TAG = "[STA_CONNECTED handler]";
 
 xEventGroupSetBits(wifi_event_group,STA_CONN);      //Set the STA_CONN flag in the Wi-Fi event group
 ESP_LOGW(TAG,"Station interface successfully connected to AP with SSID = %s, BSSID = "MACSTR", Wi-Fi Channel = %u",info->ssid,MAC2STR(info->bssid),info->channel);
 if(!WIFI_STATION_IPSTATIC)
  ESP_LOGW(TAG,"Requesting a dynamic IP configuration via the DHCP client");
 startStationNoIPDrivers();                          //Start the StationNOIP driver components, i.e. the driver components that don't require the station interface to have an IP configuration set
 printf("\n");
 return;
}

/* Raised after the Station interface disconnects from its target AP, either intentionally or due to a network error, or fails to connect to it */
void wifi_STA_DISCONNECTED_handler(system_event_sta_disconnected_t* info)
{
 static const char* TAG = "[STA_DISCONNECTED handler]";
 EventBits_t eventbits;                              //Used to check the flags in the Wi-Fi event group
 char disc_reason_str[30];                           //Used to print the reason for the disconnection as a string

 staDiscToStr(info->reason,disc_reason_str);                     //Convert the reason for the disconnection to string
 if(((eventbits = xEventGroupGetBits(wifi_event_group))>>1)&1)   //If the STA_CONN flag is set, the Station interface disconnected from its target AP
  {
   ESP_LOGW(TAG,"Station interface disconnected from the AP with SSID = %s, BSSID = "MACSTR", reason = %s, trying to reconnect...\n",info->ssid,MAC2STR(info->bssid),disc_reason_str);
   xEventGroupClearBits(wifi_event_group,STA_CONN);              //Clear the STA_CONN flag in the Wi-Fi Event Group
  }
 else                                                            //If the STA_CONN flag is not set, the Station interface failed to connect to its taget AP
  ESP_LOGW(TAG,"Station interface failed to connect to the AP with SSID = %s, reason = %s, trying again...\n",info->ssid,disc_reason_str);
 ESP_ERROR_CHECK(esp_wifi_connect());                            //Attempt to reconnect the Station Interface to its target AP
 return;
}

/* Raised once the Station interface obtains an IP configuration */
void wifi_STA_GOT_IP_handler(system_event_sta_got_ip_t* info)
{
 static const char* TAG = "[STA_GOT_IP handler]";
 tcpip_adapter_dns_info_t dns_primary;               //Used to store the address of the Station interface's Primary DNS Server
 tcpip_adapter_dns_info_t dns_secondary;             //Used to store the address of the Station interface's Secondary DNS Server

 xEventGroupSetBits(wifi_event_group,STA_GOTIP);                                                       //Set the STA_GOTIP flag in the Wi-Fi Event Group
 ESP_ERROR_CHECK(tcpip_adapter_get_dns_info(ESP_IF_WIFI_STA,TCPIP_ADAPTER_DNS_MAIN,&dns_primary));     //Retrieve the address of the Station interface primary DNS server
 ESP_ERROR_CHECK(tcpip_adapter_get_dns_info(ESP_IF_WIFI_STA,TCPIP_ADAPTER_DNS_BACKUP,&dns_secondary)); //Retrieve the address of the Station interface secondary DNS server
 if(!WIFI_STATION_IPSTATIC)                                                                            //If the Station IP configuration was retrieved dynamically via DHCP
  ESP_LOGW(TAG,"The DHCP client retrieved a dynamic IP configuration for the Station interface:");
 else                                                                                                  //Otherwise, if a static IP configuration was used for the station interface
  ESP_LOGW(TAG,"Station Interface using the following static IP configuration:");
 ESP_LOGW(TAG,"IP = "IPSTR",  NetMask = "IPSTR",  Gateway = "IPSTR,IP2STR(&info->ip_info.ip),IP2STR(&info->ip_info.netmask),IP2STR(&info->ip_info.gw));
 ESP_LOGW(TAG,"Primary DNS = "IPSTR", Secondary DNS = "IPSTR,IP2STR(&dns_primary.ip.u_addr.ip4),IP2STR(&dns_secondary.ip.u_addr.ip4));
 startStationIPDrivers();                            //Start the StationIP driver components, i.e. the driver components that require the station interface to have an IP configuration set
 printf("\n");
 return;
}

/* Raised when the lease time of the station dynamic IP configuration expires (which also implies  
   that its DHCP client failed to renew or otherwise retrieve a new IP configuration for the interface */ 
void wifi_STA_LOST_IP_handler()
{
 static const char* TAG = "[STA_LOST_IP handler]";
  
 xEventGroupClearBits(wifi_event_group,STA_GOTIP);   //Clear the STA_GOTIP flag in the Wi-Fi event group
 ESP_LOGW(TAG,"The Station dynamic IP configuration has expired\n");
 return;
}

/* Raised should the AP the Station interface is connected to change its authmode */
void wifi_STA_AUTHMODE_CHANGE_handler(system_event_sta_authmode_change_t* info)
{
 static const char* TAG = "[STA_AUTHMODE_CHANGE handler]";
 wifi_config_t sta_config;                           //Used to store the Station interface base configuration
 char ap_auth_old_str[14];                           //Used to print the old authmode of the AP the Station interface is connected to as a string
 char ap_auth_new_str[14];                           //Used to print the new authmode of the AP the Station interface is connected to as a string

 ESP_ERROR_CHECK(esp_wifi_get_config(ESP_IF_WIFI_STA,&sta_config));         //Retrieve the Station interface Base Configuration
 authToStr(info->old_mode,ap_auth_old_str);          //Convert the old authmode of the AP the Station interface is connected to to string
 authToStr(info->new_mode,ap_auth_new_str);          //Convert the new authmode of the AP the Station interface is connected to to string
 
 ESP_LOGW(TAG,"The AP the Station interface is connected to changed its authentication protocol: %s -> %s",ap_auth_old_str,ap_auth_new_str);
               
 //If the AP switched to an open authentication, or it is the target AP but its current password differs from the one set in the menuconfig utility
 if((info->new_mode == WIFI_AUTH_OPEN)||((!strcmp((char*)sta_config.sta.ssid,WIFI_STATION_AP_SSID))&&(strcmp((char*)sta_config.sta.password,WIFI_STATION_AP_PASSWORD))))
  {
   ESP_ERROR_CHECK(esp_wifi_get_config(ESP_IF_WIFI_STA,&sta_config));  //Retrieve the Station interface Base Configuration
   if(info->new_mode == WIFI_AUTH_OPEN)                                //If the AP switched to an open authentication, to allow the Station interface  
    {                                                                  //to reconnect to it the password in its base configuration must be reset 
     ESP_LOGW(TAG,"Resetting the AP password in the Station base configuration to allow it to reconnect to the AP");
     sta_config.sta.password[0] = '\0';                                       
    }
   else                                                                //Otherwise if the AP is the target AP but the current password differs from the one set in
    {                                                                  //the menuconfig utility, attempt to connect the Station interface by using such password
     ESP_LOGW(TAG,"Attempting to reconnect the Station interface using the target AP last known password (\"%s\")",WIFI_STATION_AP_PASSWORD);
     strcpy((char*)sta_config.sta.password,WIFI_STATION_AP_PASSWORD);        
    } 
   ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA,&sta_config));  //Update the password in the Station base configuration
   ESP_ERROR_CHECK(esp_wifi_disconnect());                             //Disconnect to the AP and attempt to reconnect to it (note that the actual reconnection  
  }                                                                    //attempt is performed in the SYSTEM_EVENT_STA_DISCONNECTED event specific handler)
 else                                                                  //Otherwise note that if the password relative to the new AP authmode differs from the one set in the Station base configuration, the Station 
  if(info->old_mode == WIFI_AUTH_OPEN)                                 //interface cannot reconnect to the AP (whether this is the case cannot be known in this handler except when switching from an open authentication)
   ESP_LOGE(TAG,"The Station interface cannot reconnect to the AP for it has no means to retrieve the password relative to its new authmode");
 printf("\n");     
 return;            
}    

/**====================== SoftAP Interface Events ======================**/

/* Raised after the SoftAP interface has been enabled on the device */
void wifi_AP_START_handler()
{
 static const char* TAG = "[AP_START handler]";
 wifi_config_t softap_config;                        //Used to store the SoftAP interface base configuration
 tcpip_adapter_ip_info_t ipinfo;                     //Used to store the SoftAP Interface IP Configuration
 tcpip_adapter_dns_info_t dns_primary;               //Used to store the address of the SoftAP interface's Primary DNS Server
 char softap_config_str[200];                        //Used to print the parameters of the SoftAP Base Configuration as a string

 xEventGroupSetBits(wifi_event_group,SOFTAP_ON);                                                      //Set the SOFTAP_ON flag in the Wi-Fi event group
 ESP_ERROR_CHECK(esp_wifi_get_config(ESP_IF_WIFI_AP,&softap_config));                                 //Retrieve the SoftAP interface base configuration
 ESP_ERROR_CHECK(tcpip_adapter_get_ip_info(ESP_IF_WIFI_AP,&ipinfo));                                  //Retrieve the SoftAP interface IP configuration
 ESP_ERROR_CHECK(tcpip_adapter_get_dns_info(ESP_IF_WIFI_AP,TCPIP_ADAPTER_DNS_MAIN,&dns_primary));     //Retrieve the address of the SoftAP interface primary DNS server
 softapCfgToStr(&softap_config.ap,softap_config_str);                                                 //Convert the parameters in the SoftAP interface base configuration to string

 ESP_LOGW(TAG,"SoftAP interface enabled");
 ESP_LOGW(TAG,"%s",softap_config_str);
 ESP_LOGW(TAG,"IP = "IPSTR", NetMask = "IPSTR", Gateway = "IPSTR,IP2STR(&ipinfo.ip),IP2STR(&ipinfo.netmask),IP2STR(&ipinfo.gw));
 ESP_LOGW(TAG,"Primary DNS = "IPSTR", Secondary DNS = (none)",IP2STR(&dns_primary.ip.u_addr.ip4));
 startSoftAPDrivers();                               //Start the SoftAP driver components, i.e. the driver components relative to the SoftAP interface
 printf("\n");
 return;
}

/* Raised after the SoftAP interface became disabled, which can occur either intentionally by calling 
   the esp_wifi_stop() function (not used in this example) or due to a fatal error in the Wi-Fi stack */
void wifi_AP_STOP_handler()
{
 static const char* TAG = "[AP_STOP handler]";

 xEventGroupClearBits(wifi_event_group,SOFTAP_ON);                            //Clear the SOFTAP_ON flag in the Wi-Fi Event Group
 ESP_LOGW(TAG,"SoftAP interface became disabled, trying to reenable...\n");   //Since in this example the disabling of the SoftAP interface is always unintentional, as an
 ESP_ERROR_CHECK(esp_wifi_start());                                           //error recovery attempt we to try to reenable it by calling the esp_wifi_start() function
 return;
}

/* Raised after a client connects to the SoftAP Interface */
void wifi_AP_STACONNECTED_handler(system_event_ap_staconnected_t* info)
{
 static const char* TAG = "[AP_STACONNECTED handler]";
 EventBits_t eventbits;                                                        //Used to check the flags in the Wi-Fi event group
 
 if(!(((eventbits = xEventGroupGetBits(wifi_event_group))>>4)&1))              //If in the Wi-Fi event group SOFTAP_CLI flag is not set, and thus this is the 
  xEventGroupSetBits(wifi_event_group,SOFTAP_CLI);                             //first client to connect to the SoftAP interface, set the SOFTAP_CLI flag
 ESP_LOGW(TAG,"New Client with MAC = "MACSTR" connected to the SoftAP interface, aid = %d\n",MAC2STR(info->mac),info->aid);
 return;
}

/* Raised after a client disconnects from the SoftAP Interface */
void wifi_AP_STADISCONNECTED_handler(system_event_ap_stadisconnected_t* info)
{
 static const char* TAG = "[AP_STADISCONNECTED handler]";
 wifi_sta_list_t clients;                                 //Used to store information on the clients connected to the SoftAP interface
  
 ESP_ERROR_CHECK(esp_wifi_ap_get_sta_list(&clients));     //Retrieve information on the clients connected to the SoftAP interface
 if(clients.num == 0)                                     //If no client is connected to the SoftAP interface, clear the SOFTAP_CLI flag in the Wi-Fi event group
  xEventGroupClearBits(wifi_event_group,SOFTAP_CLI);
 ESP_LOGW(TAG,"Client with MAC = "MACSTR" disconnected from the SoftAP interface, aid = %d\n",MAC2STR(info->mac),info->aid);
 return;
}

/* Raised after a connected client accepts a dynamic IP configuration offered by the SoftAP DHCP server */
/** TODO This event is not fully implemented in the current ESP-IDF version (no information on the accepted IP */
/**      configuration or the client which accepted it are passed by the Wi-Fi stack at the application level) */
void wifi_AP_STAIPASSIGNED_handler()
{
 static const char* TAG = "[AP_STAIPASSIGNED handler]";
 
 ESP_LOGW(TAG,"A client accepted a dynamic IP configuration offered by the SoftAP DHCP server\n");
 return;
}

/*************************************************************************
 *                    Wi-Fi Events General Handler
 *************************************************************************/
esp_err_t wifi_events_handler(void* ctx, system_event_t* event)
{
 static const char* TAG = "<EVENT>";
 EventBits_t eventbits;                       //Used to check the flags in the Wi-Fi event group /** Used exclusively to hotfix the STA Custom MAC bogus SYSTEM_EVENT_STA_STOP event bug described below */
 
  /* Call the specific handler relative to each Wi-Fi event raised by the Wi-Fi 
    stack, passing the provided additional information provided where applicable */
 switch(event->event_id)
  {
   /*-- Station Interface Events --*/
   case SYSTEM_EVENT_STA_START:
    ESP_LOGW(TAG,"SYSTEM_EVENT_STA_START");
    wifi_STA_START_handler();
    break;
   case SYSTEM_EVENT_STA_STOP:
    /**-----------------------------------------------------------------------  TODO BUG: Currently setting a custom MAC address to the station interface via the esp_wifi_set_mac() function causes the */
    if(!(((eventbits = xEventGroupGetBits(wifi_event_group))>>0)&1))       /**            Wi-Fi stack to raise a SYSTEM_EVENT_STA_STOP event even if the interface was not enabled in the first place,   */ 
     break;                                                                /**            and as a workround to prevent its specific handler from being called in such circumstance a check on the       */
    /**-----------------------------------------------------------------------            STA_ON flag has been added to ensure that it is called only if the Station interface is (or was) enabled)      */
    ESP_LOGW(TAG,"SYSTEM_EVENT_STA_STOP");                                                
    wifi_STA_STOP_handler();
    break;                           
   case SYSTEM_EVENT_SCAN_DONE:
    ESP_LOGW(TAG,"SYSTEM_EVENT_SCAN_DONE");
    wifi_SCAN_DONE_handler(&event->event_info.scan_done);
    break;                                           
   case SYSTEM_EVENT_STA_CONNECTED:
    ESP_LOGW(TAG,"SYSTEM_EVENT_STA_CONNECTED");
    wifi_STA_CONNECTED_handler(&event->event_info.connected);
    break;
   case SYSTEM_EVENT_STA_DISCONNECTED:
    ESP_LOGW(TAG,"SYSTEM_EVENT_STA_DISCONNECTED");
    wifi_STA_DISCONNECTED_handler(&event->event_info.disconnected);
    break;
   case SYSTEM_EVENT_STA_GOT_IP:
    ESP_LOGW(TAG,"SYSTEM_EVENT_STA_GOT_IP");
    wifi_STA_GOT_IP_handler(&event->event_info.got_ip);
    break;
   case SYSTEM_EVENT_STA_LOST_IP:
    ESP_LOGW(TAG,"SYSTEM_EVENT_STA_LOST_IP");
    wifi_STA_LOST_IP_handler();
    break;
   case SYSTEM_EVENT_STA_AUTHMODE_CHANGE:
    ESP_LOGW(TAG,"SYSTEM_EVENT_STA_AUTHMODE_CHANGE");
    wifi_STA_AUTHMODE_CHANGE_handler(&event->event_info.auth_change);
    break;
 
   /*-- SoftAP Interface Events --*/
   case SYSTEM_EVENT_AP_START:
    ESP_LOGW(TAG,"SYSTEM_EVENT_AP_START");
    wifi_AP_START_handler();
    break;
   case SYSTEM_EVENT_AP_STOP:
    ESP_LOGW(TAG,"SYSTEM_EVENT_AP_STOP");
    wifi_AP_STOP_handler();
    break;
   case SYSTEM_EVENT_AP_STACONNECTED:
    ESP_LOGW(TAG,"SYSTEM_EVENT_AP_STACONNECTED");
    wifi_AP_STACONNECTED_handler(&event->event_info.sta_connected);
    break;
   case SYSTEM_EVENT_AP_STADISCONNECTED:
    ESP_LOGW(TAG,"SYSTEM_EVENT_AP_STADISCONNECTED");
    wifi_AP_STADISCONNECTED_handler(&event->event_info.sta_disconnected);
    break;
   case SYSTEM_EVENT_AP_STAIPASSIGNED:                                     /** TODO This event is not fully implemented in the current ESP-IDF version (no information on the accepted IP */
    ESP_LOGW(TAG,"SYSTEM_EVENT_AP_STAIPASSIGNED");                         /**      configuration or the client which accepted it are passed by the Wi-Fi stack at the application level) */
    wifi_AP_STAIPASSIGNED_handler();
    break;

   default:
    ESP_LOGE(TAG,"Unknown Wi-Fi Event with ID: %u",event->event_id);
    break;
  }
 return ESP_OK;
}

/*************************************************************************
 *                       Wi-Fi Initializer Function
 *************************************************************************/
esp_err_t wifi_init()
{
 static const char* TAG = "[wifi_init]";

 //Disable the default system logging relative to the Wi-Fi stack (comment to debug)
 esp_log_level_set("system_api",ESP_LOG_NONE);  //Disable the system API default logging
 esp_log_level_set("event",ESP_LOG_NONE);       //Disable the system events default logging
 esp_log_level_set("wifi",ESP_LOG_NONE);        //Disable the Wi-Fi stack default logging
 
 printf("\n");
 ESP_LOGI(TAG,"========================== [Wi-Fi Initializer Function] ==========================");
   
 //[1/10]: Initialize the system flash storage
 esp_err_t ret = nvs_flash_init();
 if(ret == ESP_ERR_NVS_NO_FREE_PAGES)         //Error recovery attempt
  {
   ESP_ERROR_CHECK(nvs_flash_erase());
   ret = nvs_flash_init();
  }
 ESP_ERROR_CHECK(ret);
 ESP_LOGI(TAG,"[1/10] - System flash storage successfully initialized");

 //[2/10]: Initialize the TCP/LwIP stack
 tcpip_adapter_init();
 ESP_LOGI(TAG,"[2/10] - TCP/LwIP Stack successfully initialized");

 //[3/10]: Create the Wi-Fi Event Group
 wifi_event_group = xEventGroupCreate();
 ESP_LOGI(TAG,"[3/10] - Wi-Fi Event Group successfully created");

 //[4/10]: Register the Wi-Fi Events General Handler
 ESP_ERROR_CHECK(esp_event_loop_init(wifi_events_handler,NULL));
 ESP_LOGI(TAG,"[4/10] - Wi-Fi Events General Handler successfully registered");

 //[5/10]: Initialize the Wi-Fi Stack
 wifi_init_config_t initcfg = WIFI_INIT_CONFIG_DEFAULT();
 ESP_ERROR_CHECK(esp_wifi_init(&initcfg));
 ESP_LOGI(TAG,"[5/10] - Wi-Fi Stack successfully initialized");

 //[6/10]: Interface Modes Selection
 if(WIFI_STATION_ENABLE&&WIFI_SOFTAP_ENABLE)	         //If both interface modes are selected
  {
   ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_APSTA));  //Enable the Wi-Fi interface in APSTA mode
   ESP_LOGI(TAG,"[6/10] - Wi-Fi interface successfully enabled in APSTA mode");
  }
 else
  if(WIFI_STATION_ENABLE)						         //If only the Station interface mode is selected
   {
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));   //Enable the Wi-Fi interface in STA mode
    ESP_LOGI(TAG,"[6/10] - Wi-Fi interface successfully enabled in STA mode");
   }
  else
   if(WIFI_SOFTAP_ENABLE)                                //If only the SoftAP interface mode is selected
    {
     ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));   //Enable the Wi-Fi interface in SoftAP mode
     ESP_LOGI(TAG,"[6/10] - Wi-Fi interface successfully enabled in AP mode");
    }
   else                                                  //If no mode is selected for the Wi-Fi interface,  
    {                                                    //the program's execution cannot continue
     ESP_LOGE(TAG,"[6/10] - No valid mode selected for the Wi-Fi interface, aborting.");
     vTaskDelay(5000/portTICK_PERIOD_MS);
     abort();
    }

 //[7/10]: Interface Modes Base Configurations

   //Station interface Base Configuration
   if(WIFI_STATION_ENABLE)
    {
     wifi_config_t station_config = {0};                 //Used to store the Station interface base configuration
     char sta_config_str[150];                           //Used to print the parameters of the Station Base configuration as a string

     //Set the Target AP's SSID
     strcpy((char*)station_config.sta.ssid,WIFI_STATION_AP_SSID);
    
     //Set the Target AP's Password
     strcpy((char*)station_config.sta.password,WIFI_STATION_AP_PASSWORD);
    
     //Whether to connect to a target AP with a specific BSSID
     if(WIFI_STATION_USE_SPECIFIC_BSSID)
      {
       station_config.sta.bssid_set = true;
       strToMAC(WIFI_STATION_SPECIFIC_BSSID,station_config.sta.bssid);      //Convert the Target AP's specific BSSID from the menuconfig string to uint8_t[6]
      }

     //Apply the Station interface Base Configuration
     ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA,&station_config));
     staCfgToStr(&station_config.sta,sta_config_str);                       //Convert the parameters in the Station base configuration to string
     ESP_LOGI(TAG,"[7/10] - Station interface base configuration applied");
     ESP_LOGI(TAG,"  |----- Target AP Settings: %s",sta_config_str);
    }
   
   //SoftAP interface Base Configuration
   if(WIFI_SOFTAP_ENABLE)
    {
     wifi_config_t softap_config = {0};                  //Used to store the SoftAP interface base configuration
     char softap_config_str[200];                        //Used to print the parameters of the SoftAP Base Configuration as a string

     //Set the SoftAP SSID
     strcpy((char*)softap_config.ap.ssid,WIFI_SOFTAP_SSID);

     //Set the SoftAP SSID Length
     softap_config.ap.ssid_len = strlen(WIFI_SOFTAP_SSID);
 
     //Whether to hide the SoftAP SSID from its Wi-Fi beacon frames
     if(WIFI_SOFTAP_SSID_HIDDEN)
      softap_config.ap.ssid_hidden = 1;

     //Set the SoftAP Authmode
     softap_config.ap.authmode = WIFI_SOFTAP_AUTHMODE;
     
     //Set the SoftAP Password
     strcpy((char*)softap_config.ap.password,WIFI_SOFTAP_PASSWORD);

     //Whether to set the SoftAP Interface onto a specific Wi-Fi channel or let the Wi-Fi stack choose its Wi-Fi channel
     if(WIFI_SOFTAP_CHANNEL_MANUAL)
      softap_config.ap.channel = WIFI_SOFTAP_CHANNEL;

     //Set the SoftAP Maximum Connections
     softap_config.ap.max_connection = WIFI_SOFTAP_MAXCONNECTIONS;
     
     //Set the SoftAP Wi-Fi Beacon Sending Interval
     softap_config.ap.beacon_interval = WIFI_SOFTAP_BEACON_INTERVAL;

     //Apply the SoftAP interface Base Configuration
     ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP,&softap_config));
     softapCfgToStr(&softap_config.ap,softap_config_str);                   //Convert the parameters in the SoftAP interface base configuration to string
     ESP_LOGI(TAG,"[7/10] - SoftAP interface base configuration applied:");
     ESP_LOGI(TAG,"  |----- %s",softap_config_str);
    }
 
 //[8/10]: Other Interface Modes Settings
 uint8_t mac[6];                              //Possibly used to set custom MAC addresses for the station and/or the SoftAP interfaces
 
   /*--- Station Interface ---*/
   if(WIFI_STATION_ENABLE)
    {
     ESP_LOGI(TAG,"[8/10] - Other Station Interface Settings");

     //[8.1] - Whether to use a custom MAC address for the Station Interface  /** TODO BUG: Currently setting a custom MAC address to the station interface causes the Wi-Fi stack to raise a SYSTEM_EVENT_STA_STOP */
     if(WIFI_STATION_USE_CUSTOM_MAC)                                          /**           event even if the interface was not enabled in the first place, and a workround to prevent its specific handler from    */
      {                                                                       /**           being called in such circumstance has been added in the Wi-Fi Events General Handler                                    */
       ESP_ERROR_CHECK(strToMAC(WIFI_STATION_CUSTOM_MAC,mac));        //Convert the Station custom MAC address from the menuconfig string to uint8_t[6]
       ESP_ERROR_CHECK(esp_wifi_set_mac(ESP_IF_WIFI_STA,mac));        //Apply the station custom MAC address
       ESP_LOGI(TAG,"  |--[8.1] - Station interface using custom MAC address: %s",WIFI_STATION_CUSTOM_MAC);
      }
     else
      {
       ESP_ERROR_CHECK(esp_wifi_get_mac(ESP_IF_WIFI_STA,mac));
       ESP_LOGI(TAG,"  |--[8.1] - Station interface using default NIC MAC address: "MACSTR,MAC2STR(mac));
      }
 
     //[8.2] - Station Interface Power Saving Mode
     char sta_ps_str[35];                                             //Used to print the Station interface power saving mode as a string
     
     ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_STATION_POWERSAVE_MODE));   //Apply the Station interface power saving mode
     psToStr(WIFI_STATION_POWERSAVE_MODE,sta_ps_str);                 //Convert the Station interface power saving mode to string
     if(WIFI_STATION_POWERSAVE_MODE == WIFI_PS_MAX_MODEM)             //If the maximum power saving mode is used, append the listen_interval to the logging
      sprintf(sta_ps_str + strlen(sta_ps_str)," (listen_interval = %u)",WIFI_STATION_LISTEN_INTERVAL);
     ESP_LOGI(TAG,"  |--[8.2] - Station interface Power Saving Mode: %s",sta_ps_str);
    }

   /*--- SoftAP Interface ---*/
   if(WIFI_SOFTAP_ENABLE)
    {
     ESP_LOGI(TAG,"[8/10] - Other SoftAP Interface Settings");

     //[8.1] - Whether to use a custom MAC address for the SoftAP Interface
     if(WIFI_SOFTAP_USE_CUSTOM_MAC)
      {
       ESP_ERROR_CHECK(strToMAC(WIFI_SOFTAP_CUSTOM_MAC,mac));         //Convert the SoftAP custom MAC address from the menuconfig string to uint8_t[6]
       ESP_ERROR_CHECK(esp_wifi_set_mac(ESP_IF_WIFI_AP,mac));         //Apply the SoftAP custom MAC address
       ESP_LOGI(TAG,"  |--[8.1] - SoftAP interface using custom MAC address: %s",WIFI_SOFTAP_CUSTOM_MAC);
      }
     else
      {
       ESP_ERROR_CHECK(esp_wifi_get_mac(ESP_IF_WIFI_AP,mac));
       ESP_LOGI(TAG,"  |--[8.1] - SoftAP interface using default NIC MAC address: "MACSTR,MAC2STR(mac));
      }
    }

 //[9/10]: Interface Modes IP Settings
 tcpip_adapter_ip_info_t ipinfo;           //Possibly used to set a custom static IP configuration for Station and/or the SoftAP interface
 tcpip_adapter_dns_info_t dnsaddr;         //Possibly used to set the addresses of the DNS servers of the Station and/or the softAP interface

   if(WIFI_STATION_ENABLE)
    {
     //Whether to use a static IP configuration to the Station Interface
     if(WIFI_STATION_IPSTATIC)
      {
       //Stop the Station DHCP client
       ESP_ERROR_CHECK(tcpip_adapter_dhcpc_stop(TCPIP_ADAPTER_IF_STA));
      
       //Set the Station static IP configuration
       inet_pton(AF_INET,WIFI_STATION_STATIC_IP,&ipinfo.ip);
       inet_pton(AF_INET,WIFI_STATION_STATIC_NETMASK,&ipinfo.netmask);
       inet_pton(AF_INET,WIFI_STATION_STATIC_GATEWAY,&ipinfo.gw);
       ESP_ERROR_CHECK(tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_STA,&ipinfo)); 
  
       //Set the Station DNS servers
       inet_pton(AF_INET,WIFI_STATION_STATIC_DNS_PRIMARY,&dnsaddr.ip);
       ESP_ERROR_CHECK(tcpip_adapter_set_dns_info(TCPIP_ADAPTER_IF_STA,TCPIP_ADAPTER_DNS_MAIN,&dnsaddr));
       inet_pton(AF_INET,WIFI_STATION_STATIC_DNS_SECONDARY,&dnsaddr.ip);
       ESP_ERROR_CHECK(tcpip_adapter_set_dns_info(TCPIP_ADAPTER_IF_STA,TCPIP_ADAPTER_DNS_BACKUP,&dnsaddr));
    
       ESP_LOGI(TAG,"[9/10] - Station interface using the following static IP configuration:");
       ESP_LOGI(TAG,"  |----- IP = %s, Netmask = %s, Gateway = %s",WIFI_STATION_STATIC_IP,WIFI_STATION_STATIC_NETMASK,WIFI_STATION_STATIC_GATEWAY);
       ESP_LOGI(TAG,"  |----- Primary DNS = %s, Secondary DNS = %s",WIFI_STATION_STATIC_DNS_PRIMARY,WIFI_STATION_STATIC_DNS_SECONDARY);
      }
     else    
      ESP_LOGI(TAG,"[9/10] - Station interface configured to retrieve a dynamic IP configuration via DHCP once connected to its target AP");
    }

   if(WIFI_SOFTAP_ENABLE)
    {
     //Whether to use a (custom) static IP configuration to the SoftAP interface
     if(WIFI_SOFTAP_USE_CUSTOM_STATIC_IP)
      {
       //Temporarily disable the SoftAP DHCP server
       ESP_ERROR_CHECK(tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP));

       //Set the SoftAP static IP configuration
       inet_pton(AF_INET,WIFI_SOFTAP_CUSTOM_STATIC_IP,&ipinfo.ip);
       inet_pton(AF_INET,WIFI_SOFTAP_CUSTOM_STATIC_NETMASK,&ipinfo.netmask);
       inet_pton(AF_INET,WIFI_SOFTAP_CUSTOM_STATIC_GATEWAY,&ipinfo.gw);
       ESP_ERROR_CHECK(tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_AP,&ipinfo)); 
  
       //Set the SoftAP primary DNS server (also note that as of the current ESP-IDF version SoftAP interfaces don't support a secondary DNS server set)
       inet_pton(AF_INET,WIFI_SOFTAP_CUSTOM_STATIC_DNS_PRIMARY,&dnsaddr.ip);
       ESP_ERROR_CHECK(tcpip_adapter_set_dns_info(TCPIP_ADAPTER_IF_AP,TCPIP_ADAPTER_DNS_MAIN,&dnsaddr));
	   
	   //Re-enable the SoftAP DHCP server
       ESP_ERROR_CHECK(tcpip_adapter_dhcps_start(TCPIP_ADAPTER_IF_AP));

       ESP_LOGI(TAG,"[9/10] - SoftAP interface using the following custom static IP configuration:"); 
       ESP_LOGI(TAG,"  |----- IP = %s, Netmask = %s, Gateway = %s",WIFI_SOFTAP_CUSTOM_STATIC_IP,WIFI_SOFTAP_CUSTOM_STATIC_NETMASK,WIFI_SOFTAP_CUSTOM_STATIC_GATEWAY);
       ESP_LOGI(TAG,"  |----- Primary DNS = %s, Secondary DNS = (none)",WIFI_SOFTAP_CUSTOM_STATIC_DNS_PRIMARY);
      }
     else
      {
       ESP_LOGI(TAG,"[9/10] - SoftAP interface using the predefined static IP configuration:");
       ESP_LOGI(TAG,"  |----- IP = 192.168.4.1, Netmask = 255.255.255.0, Gateway = 192.168.4.1");
       ESP_LOGI(TAG,"  |----- Primary DNS Server = 0.0.0.0, Secondary DNS = (none)");
      }
    }

 //[10/10]: Enable the Wi-Fi Interface
 ESP_ERROR_CHECK(esp_wifi_start());
 ESP_LOGI(TAG,"[10/10] - Wi-Fi Interface Enabled");

 ESP_LOGI(TAG,"======================== [End Wi-Fi Initializer Function] ========================\n");
 return ESP_OK;
}