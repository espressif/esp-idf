/*************************************************************************************************************
 * Copyright © 2018 Riccardo Bertini <m_bertini@hotmail.com>                                                 *
 * Licensed under the Apache License, Version 2.0 (the "License");                                           *
 * you may not use this file except in compliance with the License.                                          *
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0                        *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is      *
 * distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 * See the License for the specific language governing permissions and limitations under the License.        *
 *************************************************************************************************************/

#ifndef WIFIBASICS_SETUP_MENUCONFIG_H
#define WIFIBASICS_SETUP_MENUCONFIG_H

/**=============== [Station Interface] ===============**/
#ifdef CONFIG_WIFI_STATION_ENABLE
 #define WIFI_STATION_ENABLE 1
#else
 #define WIFI_STATION_ENABLE 0
#endif

/**--- Station Base Configuration ---**/
#ifdef CONFIG_WIFI_STATION_AP_SSID
 #define WIFI_STATION_AP_SSID CONFIG_WIFI_STATION_AP_SSID
#else
 #define WIFI_STATION_AP_SSID "0.0.0.0"
#endif

#ifdef CONFIG_WIFI_STATION_AP_PASSWORD
 #define WIFI_STATION_AP_PASSWORD CONFIG_WIFI_STATION_AP_PASSWORD
#else
 #define WIFI_STATION_AP_PASSWORD ""
#endif

#ifdef CONFIG_WIFI_STATION_USE_SPECIFIC_BSSID
 #define WIFI_STATION_USE_SPECIFIC_BSSID 1
#else
 #define WIFI_STATION_USE_SPECIFIC_BSSID 0
#endif

#ifdef CONFIG_WIFI_STATION_SPECIFIC_BSSID
 #define WIFI_STATION_SPECIFIC_BSSID CONFIG_WIFI_STATION_SPECIFIC_BSSID
#else
 #define WIFI_STATION_SPECIFIC_BSSID "00:00:00:00:00:00"
#endif

/**--- Other Station Settings ---**/
#ifdef CONFIG_WIFI_STATION_USE_CUSTOM_MAC  /** TODO BUG: Currently setting a custom MAC address to the station interface causes the Wi-Fi     */
 #define WIFI_STATION_USE_CUSTOM_MAC 1     /**           stack  to raise a  SYSTEM_EVENT_STA_STOP event even if the interface was not         */
#else                                      /**           enabled in the first place, and a workround to prevent its specific handler from     */
 #define WIFI_STATION_USE_CUSTOM_MAC 0     /**           being called in such circumstance has been added in the Wi-Fi Events General Handler */
#endif

#ifdef CONFIG_WIFI_STATION_CUSTOM_MAC
 #define WIFI_STATION_CUSTOM_MAC CONFIG_WIFI_STATION_CUSTOM_MAC
#else
 #define WIFI_STATION_CUSTOM_MAC "00:00:00:00:00:00"
#endif

#ifdef CONFIG_WIFI_STATION_PS_NONE
 #define WIFI_STATION_POWERSAVE_MODE WIFI_PS_NONE
#else
 #ifdef CONFIG_WIFI_STATION_PS_MIN
  #define WIFI_STATION_POWERSAVE_MODE WIFI_PS_MIN_MODEM
 #else
  #ifdef CONFIG_WIFI_STATION_PS_MAX
   #define WIFI_STATION_POWERSAVE_MODE WIFI_PS_MAX_MODEM
  #else
   #define WIFI_STATION_POWERSAVE_MODE WIFI_PS_MIN_MODEM
  #endif
 #endif
#endif

#ifdef CONFIG_WIFI_STATION_LISTEN_INTERVAL
 #define WIFI_STATION_LISTEN_INTERVAL CONFIG_WIFI_STATION_LISTEN_INTERVAL
#else
 #define WIFI_STATION_LISTEN_INTERVAL 3
#endif

/**--- Station IP Configuration ---**/
#ifdef CONFIG_WIFI_STATION_IPSTATIC
 #define WIFI_STATION_IPSTATIC 1
#else
 #define WIFI_STATION_IPSTATIC 0
#endif

#ifdef CONFIG_WIFI_STATION_STATIC_IP
 #define WIFI_STATION_STATIC_IP CONFIG_WIFI_STATION_STATIC_IP
#else
 #define WIFI_STATION_STATIC_IP "0.0.0.0"
#endif

#ifdef CONFIG_WIFI_STATION_STATIC_NETMASK
 #define WIFI_STATION_STATIC_NETMASK CONFIG_WIFI_STATION_STATIC_NETMASK
#else
 #define WIFI_STATION_STATIC_NETMASK "0.0.0.0"
#endif

#ifdef CONFIG_WIFI_STATION_STATIC_GATEWAY
 #define WIFI_STATION_STATIC_GATEWAY CONFIG_WIFI_STATION_STATIC_GATEWAY
#else
 #define WIFI_STATION_STATIC_GATEWAY "0.0.0.0"
#endif

#ifdef CONFIG_WIFI_STATION_STATIC_DNS_PRIMARY
 #define WIFI_STATION_STATIC_DNS_PRIMARY CONFIG_WIFI_STATION_STATIC_DNS_PRIMARY
#else
 #define WIFI_STATION_STATIC_DNS_PRIMARY "8.8.8.8"
#endif

#ifdef CONFIG_WIFI_STATION_STATIC_DNS_SECONDARY
 #define WIFI_STATION_STATIC_DNS_SECONDARY CONFIG_WIFI_STATION_STATIC_DNS_SECONDARY
#else
 #define WIFI_STATION_STATIC_DNS_SECONDARY "8.8.4.4"
#endif

/**--- Preliminary Wi-Fi Scan ---**/
#ifdef CONFIG_WIFI_STATION_SCAN_PRELIMINARY
 #define WIFI_STATION_SCAN_PRELIMINARY 1
#else
 #define WIFI_STATION_SCAN_PRELIMINARY 0
#endif

#ifdef CONFIG_WIFI_STATION_SCAN_HIDDEN
 #define WIFI_STATION_SCAN_HIDDEN 1
#else
 #define WIFI_STATION_SCAN_HIDDEN 0
#endif

#ifdef CONFIG_WIFI_STATION_SCAN_ALLOW_FALLBACK
 #define WIFI_STATION_SCAN_ALLOW_FALLBACK 1
#else
 #define WIFI_STATION_SCAN_ALLOW_FALLBACK 0
#endif

#ifdef CONFIG_WIFI_STATION_SCAN_RETRY_INTERVAL
 #define WIFI_STATION_SCAN_RETRY_INTERVAL CONFIG_WIFI_STATION_SCAN_RETRY_INTERVAL
#else
 #define WIFI_STATION_SCAN_RETRY_INTERVAL 0
#endif

/**=============== [SoftAP Interface] ===============**/
#ifdef CONFIG_WIFI_SOFTAP_ENABLE
 #define WIFI_SOFTAP_ENABLE 1
#else
 #define WIFI_SOFTAP_ENABLE 0
#endif

/**--- SoftAP Base Configuration ---**/
#ifdef CONFIG_WIFI_SOFTAP_SSID
 #define WIFI_SOFTAP_SSID CONFIG_WIFI_SOFTAP_SSID
#else
 #define WIFI_SOFTAP_SSID "ESP32 Wi-Fi"
#endif

#ifdef CONFIG_WIFI_SOFTAP_SSID_HIDDEN
 #define WIFI_SOFTAP_SSID_HIDDEN 1
#else
 #define WIFI_SOFTAP_SSID_HIDDEN 0
#endif

#ifdef CONFIG_WIFI_SOFTAP_AUTH_OPEN
 #define WIFI_SOFTAP_AUTHMODE WIFI_AUTH_OPEN
#else
 #ifdef CONFIG_WIFI_SOFTAP_AUTH_WPA_PSK
  #define WIFI_SOFTAP_AUTHMODE WIFI_AUTH_WPA_PSK
 #else
  #ifdef CONFIG_WIFI_SOFTAP_AUTH_WPA2_PSK
   #define WIFI_SOFTAP_AUTHMODE WIFI_AUTH_WPA2_PSK
  #else
   #ifdef CONFIG_WIFI_SOFTAP_AUTH_WPA_WPA2_PSK
    #define WIFI_SOFTAP_AUTHMODE WIFI_AUTH_WPA_WPA2_PSK
   #else
    #define WIFI_SOFTAP_AUTHMODE WIFI_AUTH_OPEN
   #endif
  #endif
 #endif
#endif

#ifdef CONFIG_WIFI_SOFTAP_PASSWORD
 #define WIFI_SOFTAP_PASSWORD CONFIG_WIFI_SOFTAP_PASSWORD
#else
 #define WIFI_SOFTAP_PASSWORD ""
#endif

#ifdef CONFIG_WIFI_SOFTAP_CHANNEL_MANUAL
 #define WIFI_SOFTAP_CHANNEL_MANUAL 1
#else
 #define WIFI_SOFTAP_CHANNEL_MANUAL 0
#endif

#ifdef CONFIG_WIFI_SOFTAP_CHANNEL
 #define WIFI_SOFTAP_CHANNEL CONFIG_WIFI_SOFTAP_CHANNEL
#else
 #define WIFI_SOFTAP_CHANNEL 0
#endif
 
#ifdef CONFIG_WIFI_SOFTAP_MAXCONNECTIONS
 #define WIFI_SOFTAP_MAXCONNECTIONS CONFIG_WIFI_SOFTAP_MAXCONNECTIONS
#else
 #define WIFI_SOFTAP_MAXCONNECTIONS 4
#endif

#ifdef CONFIG_WIFI_SOFTAP_BEACON_INTERVAL
 #define WIFI_SOFTAP_BEACON_INTERVAL CONFIG_WIFI_SOFTAP_BEACON_INTERVAL
#else
 #define WIFI_SOFTAP_BEACON_INTERVAL 0
#endif

/**--- Other SoftAP Settings ---**/
#ifdef CONFIG_WIFI_SOFTAP_USE_CUSTOM_MAC
 #define WIFI_SOFTAP_USE_CUSTOM_MAC 1
#else
 #define WIFI_SOFTAP_USE_CUSTOM_MAC 0
#endif

#ifdef CONFIG_WIFI_SOFTAP_CUSTOM_MAC
 #define WIFI_SOFTAP_CUSTOM_MAC CONFIG_WIFI_SOFTAP_CUSTOM_MAC
#else
 #define WIFI_SOFTAP_CUSTOM_MAC "00:00:00:00:00:00"
#endif

/**--- SoftAP IP Configuration ---**/
#ifdef CONFIG_WIFI_SOFTAP_USE_CUSTOM_STATIC_IP
 #define WIFI_SOFTAP_USE_CUSTOM_STATIC_IP 1
#else
 #define WIFI_SOFTAP_USE_CUSTOM_STATIC_IP 0
#endif

#ifdef CONFIG_WIFI_SOFTAP_CUSTOM_STATIC_IP
 #define WIFI_SOFTAP_CUSTOM_STATIC_IP CONFIG_WIFI_SOFTAP_CUSTOM_STATIC_IP
#else
 #define WIFI_SOFTAP_CUSTOM_STATIC_IP "0.0.0.0"
#endif

#ifdef CONFIG_WIFI_SOFTAP_CUSTOM_STATIC_NETMASK
 #define WIFI_SOFTAP_CUSTOM_STATIC_NETMASK CONFIG_WIFI_SOFTAP_CUSTOM_STATIC_NETMASK
#else
 #define WIFI_SOFTAP_CUSTOM_STATIC_NETMASK "0.0.0.0"
#endif

#ifdef CONFIG_WIFI_SOFTAP_CUSTOM_STATIC_GATEWAY
 #define WIFI_SOFTAP_CUSTOM_STATIC_GATEWAY CONFIG_WIFI_SOFTAP_CUSTOM_STATIC_GATEWAY
#else
 #define WIFI_SOFTAP_CUSTOM_STATIC_GATEWAY "0.0.0.0"
#endif

#ifdef CONFIG_WIFI_SOFTAP_CUSTOM_STATIC_DNS_PRIMARY
 #define WIFI_SOFTAP_CUSTOM_STATIC_DNS_PRIMARY CONFIG_WIFI_SOFTAP_CUSTOM_STATIC_DNS_PRIMARY
#else
 #define WIFI_SOFTAP_CUSTOM_STATIC_DNS_PRIMARY "0.0.0.0"
#endif


#endif /*-- WIFIBASICS_SETUP_MENUCONFIG_H --*/