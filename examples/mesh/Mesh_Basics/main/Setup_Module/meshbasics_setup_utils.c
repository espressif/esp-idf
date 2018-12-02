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
#include "meshbasics_setup_menuconfig.h"          //Setup Module menuconfig parameters

/*************************************************************************
 *                         Functions Definitions
 *************************************************************************/

/* Resets the node's mesh status and the flags in the mesh event group to a set of default
   parameters representing a node that is not connected to a parent in the mesh network 
   (called in the MESH_EVENT_STARTED, MESH_EVENT_STOPPED and MESH_EVENT_PARENT DISCONNECTED events specific handlers) */
void mesh_reset_status()
{                            
 EventBits_t eventbits;                                            //Used to check the flags of the mesh event group
  
 memset(mesh_state.root_addr,0,6);                                 //Reset the root's address                       
 memset(mesh_state.parent_addr,0,6);                               //Reset the node's parent address
 mesh_state.my_layer = -1;                                         //Reset the node's mesh layer 
 if(((eventbits = xEventGroupGetBits(mesh_event_group))>>2)&1)     //Reset the node's type depending on it having children connected or not
  mesh_state.my_type = MESH_NODE;
 else
  mesh_state.my_type = MESH_IDLE;
 xEventGroupClearBits(mesh_event_group,MESH_PARENT|MESH_TODS);     //Clear the MESH_PARENT and MESH_TODS flags in the mesh event group
 xEventGroupSetBits(mesh_event_group,MESH_VOTE);                   //Set the MESH_VOTE flag in the mesh event group (since a node whose mesh status has been reset is no longer able to partecipate in an occuring election and thus  
 return;                                                           //to raise the MESH_EVENT_VOTE_STOPPED_EVENT, to ensure the coherency of this flag each node must assume that every election ends when its mesh status is reset)
}

/* Resets the root node's station IP configuration (and implicitly its DNS servers)
   (called in the MESH_EVENT_PARENT_DISCONNECTED, MESH_EVENT_ROOT_SWITCH_REQ and MESH_EVENT_ROOT_ASKED_YIELD events specific handlers) */
void mesh_root_reset_stationIP()                                   
{
 tcpip_adapter_ip_info_t ipinfo;                                   //Used to set an interface's IP configuration

 if(MESH_ROOT_IPDYNAMIC)                                           //If the root node used a static IP configuration on its station interface, its station DHCP client
  ESP_ERROR_CHECK(tcpip_adapter_dhcpc_stop(TCPIP_ADAPTER_IF_STA)); //must be stopped to prevent an error from happening later should the node become root again
 
 //Reset the root node's station IP configuration
 inet_pton(AF_INET,"0.0.0.0",&ipinfo.ip);
 inet_pton(AF_INET,"0.0.0.0",&ipinfo.netmask);
 inet_pton(AF_INET,"0.0.0.0",&ipinfo.gw);
 ESP_ERROR_CHECK(tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_STA,&ipinfo)); //Apply the set IP configuration to the root node's station interface, thus resetting it
 return;
}

/* Checks whether two arrays are equal
   (used in the MESH_EVENT_SCAN_DONE event specific handler)  */
bool checkArrayEqual(uint8_t* arr1, uint8_t* arr2, uint8_t size)
{
 for(unsigned int i=0;i<size;i++)
  if(arr1[i]!=arr2[i])
   return false;                  //The two arrays are NOT equal
 return true;                     //The two arrays are equal
}

/* Converts the reason a connection a parent cannot be attempted to string
   (used in the MESH_EVENT_SCAN_DONE event specific handler)               */
void connErrToStr(int status, char* dest)
{
 switch(status)
  {
   case 1:
    strcpy(dest,", but belongs to another mesh network");
    break;
   case 2:
    strcpy(dest,", but it's currently in the idle status");
    break; 
   case 3:
    strcpy(dest,", but is a leaf node");
    break;
   case 4:
    strcpy(dest,", but its maximum children capacity has been reached");
    break;
   case 5:
    strcpy(dest,", but it has a too low RSSI to attempt to connect to");
    break;
   default:
    break;
  }
 return;
}

/* Sets the target of a parent connection attempt to a specific node
   (used in the MESH_EVENT_SCAN_DONE event specific handler)         */
void setParentNode(wifi_ap_record_t* node_rec, mesh_assoc_t* node_ie, wifi_sta_config_t* parent, mesh_type_t* my_type, int* my_layer)
{
 strcpy((char*)parent->ssid,(char*)node_rec->ssid);  //Set the AP's SSID 
 parent->password[0] = '\0';                         //Set the AP's password (none)
 parent->bssid_set = 1;                              //Define that the node should connect to an AP with a specific BSSID only
 memcpy(parent->bssid,node_rec->bssid,6);            //Set the AP's BSSID
 parent->channel = MESH_WIFI_CHANNEL;                //Set the AP's Wi-Fi channel
 *my_layer = node_ie->layer+1;                       //Set the node's supposed mesh layer should it successfully connect to its parent 
 if(*my_layer<MESH_MAX_LAYER)                        //Set the node's supposed type should it successfully connect to its parent
  *my_type = MESH_NODE;
 else
  *my_type = MESH_LEAF;
 return;
}

/* Sets the target of a parent connection attempt to a specific node
   (used in the MESH_EVENT_SCAN_DONE event specific handler)         */
void setParentRouter(wifi_ap_record_t* router_rec, wifi_sta_config_t* parent, mesh_type_t* my_type, int* my_layer)
{
 strcpy((char*)parent->ssid,(char*)router_rec->ssid);   //Set the AP's SSID
 strcpy((char*)parent->password,MESH_ROUTER_PASSWORD);  //Set the AP's password
 parent->bssid_set = 1;                                 //Define that the node should connect to an AP with a specific BSSID only
 memcpy(parent->bssid,router_rec->bssid,6);             //Set the AP's BSSID
 parent->channel = MESH_WIFI_CHANNEL;                   //Set the AP's Wi-Fi channel
 *my_layer = 1;                                         //Set the node's supposed mesh layer should it successfully connect to its parent
 *my_type = MESH_ROOT;                                  //Set the node's supposed type should it successfully connect to its parent
 return;
}

/* Converts the reason a node disconnected from its parent to string
   (called in the MESH_EVENT_PARENT_DISCONNECTED event specific handler) */
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

/* Converts the reason for a new root election in the mesh network to string 
   (called in the MESH_EVENT_VOTE_STARTED event specific handler)            */
void voteReasonToStr(unsigned int reason, char* dest)
{ 
 dest[0] = '\0';                                      
 switch(reason)
  {
   case MESH_VOTE_REASON_ROOT_INITIATED:
    strcpy(dest,"Root Waive");
    break;
   case MESH_VOTE_REASON_CHILD_INITIATED:
    strcpy(dest,"Root Self-Healing");
    break;
   default:
    sprintf(dest,"UNKNOWN (%u)",reason);
    break;
  }
 return;
}