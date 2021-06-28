/*************************************************************************************************************
 * Copyright © 2018 Riccardo Bertini <m_bertini@hotmail.com>                                                 *
 * Licensed under the Apache License, Version 2.0 (the "License");                                           *
 * you may not use this file except in compliance with the License.                                          *
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0                        *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is      *
 * distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 * See the License for the specific language governing permissions and limitations under the License.        *
 *************************************************************************************************************/

#ifndef WIFIBASICS_DRIVER_MENUCONFIG_H
#define WIFIBASICS_DRIVER_MENUCONFIG_H

/**========== [StationNOIP Logging Driver] ==========**/
#ifdef CONFIG_WIFI_DRIVER_STATION_NOIP_LOGGING_ENABLE
 #define WIFI_DRIVER_STATION_NOIP_LOGGING_ENABLE 1
#else
 #define WIFI_DRIVER_STATION_NOIP_LOGGING_ENABLE 0
#endif

#ifdef CONFIG_WIFI_DRIVER_STATION_NOIP_LOGGING_INTERVAL
 #define WIFI_DRIVER_STATION_NOIP_LOGGING_INTERVAL CONFIG_WIFI_DRIVER_STATION_NOIP_LOGGING_INTERVAL
#else
 #define WIFI_DRIVER_STATION_NOIP_LOGGING_INTERVAL 0
#endif

/**=========== [StationIP Logging Driver] ===========**/
#ifdef CONFIG_WIFI_DRIVER_STATION_IP_LOGGING_ENABLE
 #define WIFI_DRIVER_STATION_IP_LOGGING_ENABLE 1
#else
 #define WIFI_DRIVER_STATION_IP_LOGGING_ENABLE 0
#endif

#ifdef CONFIG_WIFI_DRIVER_STATION_IP_LOGGING_INTERVAL
 #define WIFI_DRIVER_STATION_IP_LOGGING_INTERVAL CONFIG_WIFI_DRIVER_STATION_IP_LOGGING_INTERVAL
#else
 #define WIFI_DRIVER_STATION_IP_LOGGING_INTERVAL 0
#endif

/**============= [SoftAP Logging Driver] =============**/
#ifdef CONFIG_WIFI_DRIVER_SOFTAP_LOGGING_ENABLE
 #define WIFI_DRIVER_SOFTAP_LOGGING_ENABLE 1
#else
 #define WIFI_DRIVER_SOFTAP_LOGGING_ENABLE 0
#endif

#ifdef CONFIG_WIFI_DRIVER_SOFTAP_LOGGING_CLIENTS
 #define WIFI_DRIVER_SOFTAP_LOGGING_CLIENTS 1
#else
 #define WIFI_DRIVER_SOFTAP_LOGGING_CLIENTS 0
#endif

#ifdef CONFIG_WIFI_DRIVER_SOFTAP_LOGGING_INTERVAL
 #define WIFI_DRIVER_SOFTAP_LOGGING_INTERVAL CONFIG_WIFI_DRIVER_SOFTAP_LOGGING_INTERVAL
#else
 #define WIFI_DRIVER_SOFTAP_LOGGING_INTERVAL 0
#endif


#endif /*-- WIFIBASICS_DRIVER_MENUCONFIG_H --*/