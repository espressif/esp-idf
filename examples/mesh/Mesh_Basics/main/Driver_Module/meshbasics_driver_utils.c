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
#include "meshbasics_driver_menuconfig.h"         //Driver Module menuconfig parameters

/*************************************************************************
 *                         Functions Definitions
 *************************************************************************/

/* Converts a Mesh Organization Mode to string
   (used in the Logging driver when logging the node's mesh status) */
void orgToStr(mesh_org_t org, char* dest)
{
 switch(org)                                               
  {
   case SELF_ORGANIZED:
    strcpy(dest,"SELF_ORGANIZED");
    break;
   case FIXED_ROOT:
    strcpy(dest,"FIXED_ROOT");
    break;
   case MANUAL_NETWORKING:
    strcpy(dest,"MANUAL_NETWORKING");
    break;
   default:
    strcpy(dest,"UNKNOWN");
    break;
  }
 return;
}

/* Converts the flags relative to the Setup Module in the mesh event group to string 
   (used in the Logging Driver when logging the node's mesh event group)             */
void eventGroupSetupToStr(char* dest)
{
 EventBits_t eventbits;                             //Used to check the flags in the mesh event group
 
 dest[0] = '\0';                                      
 eventbits = xEventGroupGetBits(mesh_event_group);  //Retrieve the value of the mesh event group
 if((eventbits>>4)&1)                               //For each flag relative to the setup module set in the mesh event group, append its name to the string
  strcat(dest,"MESH_VOTE, ");
 if((eventbits>>3)&1)
  strcat(dest,"MESH_TODS, ");
 if((eventbits>>2)&1)
  strcat(dest,"MESH_CHILD, ");
 if((eventbits>>1)&1)
  strcat(dest,"MESH_PARENT, ");
 if((eventbits>>0)&1)
  strcat(dest,"MESH_ON, ");
 if(strlen(dest)>0)                                 //Remove the trailing comma and space, if any
  dest[strlen(dest)-2] = '\0';
 else
  strcpy(dest,"(none)");
 return;
}

/* Converts the flags relative to the driver components executions in the mesh event group to string 
   (used in the Logging Driver when logging the node's mesh event group)                             */
void eventGroupDriverToStr(char* dest)
{
 EventBits_t eventbits;                             //Used to check the flags in the mesh event group
 
 dest[0] = '\0';                                      
 eventbits = xEventGroupGetBits(mesh_event_group);  //Retrieve the value of the mesh event group
 if((eventbits>>10)&1)                              //For each flag relative to the driver components executions set in the mesh event group, append its name to the string
  strcat(dest,"MESH_DRIVER_ROOT_WAIVE, ");          
 if((eventbits>>9)&1)                                                 
  strcat(dest,"MESH_DRIVER_ROOT_RECEIVERDS, ");
 if((eventbits>>8)&1)
  strcat(dest,"MESH_DRIVER_EXTERNAL_SENDERDS, ");
 if((eventbits>>7)&1)
  strcat(dest,"MESH_DRIVER_INTERNAL_RECEIVER, ");
 if((eventbits>>6)&1)
  strcat(dest,"MESH_DRIVER_INTERNAL_SENDER, ");
 if((eventbits>>5)&1)
  strcat(dest,"MESH_DRIVER_INTERNAL_LOGGING, ");
 if(strlen(dest)>0)                                 //Remove the trailing comma and space, if any
  dest[strlen(dest)-2] = '\0';
 else
  strcpy(dest,"(none)");
 return;
}

/* Checks whether a MAC address corresponds to the MAC address of one of the node's children
   (used in the Logging Driver when logging the node's routing table)                        */
bool checkIfChild(uint8_t* addr, wifi_sta_list_t* children)
{
 for(int i=0;i<children->num;i++)
  if((children->sta[i].mac[0] == addr[0])&&(children->sta[i].mac[1] == addr[1])&&(children->sta[i].mac[2] == addr[2])&&(children->sta[i].mac[3] == addr[3])&&(children->sta[i].mac[4] == addr[4])&&(children->sta[i].mac[5] == addr[5]))
   return true;
 return false;
} 

/* Converts a station interface's power saving mode to string
   (used in the Logging driver when logging information on the node's station interface) */
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

/* Used to wait for a child to connect to the node before resuming the execution
   (used in the Sender Driver when sending to a node's descendants)              */
void waitForChildren()
{
 const static char* TAG = "[Sender_Driver]";
 EventBits_t eventbits;                                             //Used to check the flags in the mesh event group
      
 if(!(((eventbits = xEventGroupGetBits(mesh_event_group))>>2)&1))   //If the node has no children connected (MESH_CHILD)
  {
   ESP_LOGI(TAG,"Waiting for a child node to connect before resuming the Sender Driver");
   ESP_LOGI(TAG,"=====================================================================================\n");
   xEventGroupWaitBits(mesh_event_group,MESH_CHILD,pdFALSE,pdTRUE,portMAX_DELAY);                           //Wait indefinetely for a child to connect to the node (MESH_CHILD) before resuming the Sender Driver
   ESP_LOGI(TAG,"===================================[Sender Driver]===================================");
  }
 return;
}

/* Converts an esp_mesh_send() sending mode to string
   (used in the Sender, Receiver and SenderDS drivers when logging the result of the send operation) */
void sendModeToStr(unsigned int mode, char* dest)
{
 switch(mode)
  {
   case SEND_BLOCK:
    strcpy(dest,"Blocking");
    break;
   case SEND_NONBLOCK:             /** TODO BUG: Currently using this sending mode still causes the esp_mesh_send() function to block if it would block in the first place      */
    strcpy(dest,"Non-Blocking");   /**           (it should also be noted that the Mesh API currently don't offer any method to set a timeout for the esp_mesh_send() function) */
    break;
   default:
    strcpy(dest,"UNKNOWN");
    break;
  }
 return;
}

/* Converts the protocol of the payload of a mesh packet to string
   (used in the Sender, Receiver, SenderDS and ReceiverDS drivers when logging the result of the send or receive operation) */
void proToStr(mesh_proto_t proto, char* dest)
{ 
 switch(proto)
  {
   case MESH_PROTO_BIN:
    strcpy(dest,"Binary");
    break;
   case MESH_PROTO_HTTP:
    strcpy(dest,"HTTP");
    break;
   case MESH_PROTO_JSON:
    strcpy(dest,"JSON");
    break;
   case MESH_PROTO_MQTT:
    strcpy(dest,"MQTT");
    break;
   default:
    strcpy(dest,"UNKNOWN");
    break;
  }
 return;
}

/* Converts the type of service that must be used or was used to deliver a mesh packet to string
   (used in the Sender, Receiver, SenderDS and ReceiverDS drivers when logging the result of the send or receive operation) */
void tosToStr(mesh_tos_t tos, char* dest)
{
 switch(tos)
  {
   case MESH_TOS_P2P:
    strcpy(dest,"Reliable");
    break;
   case MESH_TOS_DEF:              /** TODO BUG: Currently using the unreliable TOS causes the esp_mesh_send() function to fail if the packet's destination address */
    strcpy(dest,"Unreliable");     /**           is not found in the routing table of a node (packet that so would be forwarded upstream to the node's parent)      */
    break;
   default:
    strcpy(dest,"UNKNOWN");
    break;
  }
 return;
}

/* Converts the bits set in the flag of a mesh packet to string
   (used in the Sender, Receiver, SenderDS and ReceiverDS drivers when logging the result of the send or receive operation) */
void commFlagToStr(int flag, char* dest)
{
 dest[0] = '\0';                            
 if((flag>>1)&1)                                  
  strcat(dest,"MESH_DATA_P2P + ");       //For each bit set in the flag, append its name to the string
 if((flag>>2)&1)
  strcat(dest,"MESH_DATA_FROMDS + ");
 if((flag>>3)&1)
  strcat(dest,"MESH_DATA_TODS + ");
 if((flag>>4)&1)
  strcat(dest,"MESH_DATA_NONBLOCK + ");
 if((flag>>5)&1)
  strcat(dest,"MESH_DATA_DROP + ");
 if((flag>>6)&1)
  strcat(dest,"MESH_DATA_GROUP + ");
 
 if(strlen(dest)>3)                      //Remove the trailing space and plus sign, if any
  dest[strlen(dest)-3] = '\0';
 else
  strcpy(dest,"(none)");
 return;
}

/* Converts the type and value of an additional option in a mesh packet to string
   (used in the Sender, Receiver, SenderDS and ReceiverDS drivers when logging the result of the send or receive operation) */
void commOptToStr(mesh_opt_t* opt, uint8_t* opt_buf, char* dest)
{
 switch(opt->type)
  {
   case MESH_OPT_SEND_GROUP:
    strcpy(dest,"MESH_OPT_SEND_GROUP");
    break;
   case MESH_OPT_RECV_DS_ADDR:
    strcpy(dest,"MESH_OPT_RECV_DS_ADDR");
    break;
   case 0:
    strcpy(dest,"(none)");
    break;
   default:
    strcpy(dest,"UNKNOWN");
    break;
  }
 if(opt->type)                           //Append the option's value, if any
  {
   opt_buf[opt->len] = '\0';             //Ensure that the option's value can be parsed as string
   sprintf(dest+strlen(dest),", Option Value = %s",(char*)opt_buf);
  }
 return;
}

/* Converts the result of a send or receive operation to string
   (used in the Sender, Receiver, SenderDS and ReceiverDS drivers when logging the result of the send or receive operation) */
void commErrToStr(esp_err_t send_err, char* dest)
{
 dest[0] = '\0';  
 switch(send_err)
  {
   case ESP_ERR_MESH_ARGUMENT:
    strcat(dest,"ESP_ERR_MESH_ARGUMENT");
    break;
   case ESP_ERR_MESH_NOT_START:
    strcat(dest,"ESP_ERR_MESH_NOT_START");
    break;
   case ESP_ERR_MESH_DISCONNECTED:
    strcat(dest,"ESP_ERR_MESH_DISCONNECTED");
    break;
   case ESP_ERR_MESH_OPTION_UNKNOWN:
    strcat(dest,"ESP_ERR_MESH_OPTION_UNKNOWN");
    break;
   case ESP_ERR_MESH_EXCEED_MTU:
    strcat(dest,"ESP_ERR_MESH_EXCEED_MTU");
    break;
   case ESP_ERR_MESH_NO_MEMORY:
    strcat(dest,"ESP_ERR_MESH_NO_MEMORY");
    break;
   case ESP_ERR_MESH_TIMEOUT:
    strcat(dest,"ESP_ERR_MESH_TIMEOUT");
    break;
   case ESP_ERR_MESH_QUEUE_FULL:
    strcat(dest,"ESP_ERR_MESH_QUEUE_FULL");
    break;
   case ESP_ERR_MESH_NO_ROUTE_FOUND:
    strcat(dest,"ESP_ERR_MESH_NO_ROUTE_FOUND");
    break;
   case ESP_ERR_MESH_DISCARD:
    strcat(dest,"ESP_ERR_MESH_DISCARD");
    break;
   case ESP_FAIL:
    strcat(dest,"ESP_FAIL (unknown error)");
    break;
   default:
    sprintf(dest,"UNKNOWN (%u)",send_err);
    break;
  }
 return;
}

/* Used to wait for the node to be connected to a parent (MESH_PARENT), for the external DS to be  
   accessible (MESH_TODS) and for a new root election to finish (MESH_VOTE) before resuming the execution
   (used in the SenderDS Driver before sending each packet)                                               */
void waitForNoVote()
{
 const static char* TAG = "[SenderDS_Driver]";
 EventBits_t eventbits;                                           //Used to check the flags in the mesh event group   

 if(!(((eventbits = xEventGroupGetBits(mesh_event_group))>>4)&1)) //If a new root election is currently in progress
  {
   ESP_LOGI(TAG,"Waiting for the new root node election to end before resuming the SenderDS Driver");
   ESP_LOGI(TAG,"===================================================================================\n");
   xEventGroupWaitBits(mesh_event_group,MESH_PARENT|MESH_TODS|MESH_VOTE,pdFALSE,pdTRUE,portMAX_DELAY);    //Wait indefinetely for the node to be connected to a parent (MESH_PARENT), for the external DS to be
                                                                                                          //accessible (MESH_TODS) and for a new root election to finish (MESH_VOTE)
   vTaskDelay((esp_random()%3000)/portTICK_PERIOD_MS);                                                    //To limit the chances of packet overflows on a newly elected root node, once the above conditions are met a node
   ESP_LOGI(TAG,"=================================[SenderDS Driver]=================================");   //will wait for an additional random amount of time between 0 and 3000ms before resuming the SenderDS Driver
  }
 return;
}