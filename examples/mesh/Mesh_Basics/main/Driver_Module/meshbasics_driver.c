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
#include "meshbasics_driver_utils.c"              //Driver Module utility functions
#include "meshbasics_driver_menuconfig.h"         //Driver Module menuconfig parameters

/*************************************************************************
 *             Logging Driver (MESH_DRIVER_INTERNAL_LOGGING)
 *************************************************************************/
void meshLoggingDriver()
{
 const static char* TAG = "[Logging_Driver]";
 
 /*-- Mesh Status Logging Variables--*/
 EventBits_t eventbits;                            //Used to check the flags in the mesh event group
 uint8_t node_mac[6];                              //Used to store the node's (station) MAC address
 char node_type_str[20];                           //Used to print the node's type as a string
 char mesh_org_str[18];                            //Used to print the node's mesh organization mode as a string
 uint8_t node_group[6] = {0};                      //Used to store the mesh group the node belongs to, if any

 /* Mesh Event Group Logging Variables */
 char event_group_setup_str[55];                   //Used to print the flags in the mesh event group relative to the setup module as a string
 char event_group_driver_str[175];                 //Used to print the flags in the mesh event group relative to the driver components' executions as a string 
 
 /*-- Routing Table Logging Variables--*/
 wifi_sta_list_t children;                         //Used to store information on the connected children nodes
 mesh_addr_t* rtable;                              //Used to dynamically store the node's routing table
 int rtable_entries;                               //Used to store the number of entries in the node's routing table 

 /*-- Station + SoftAP interfaces Logging Variables --*/
 uint8_t mac[6];                                   //Used to store the MAC addresses of the node's station and/or SoftAP interfaces
 wifi_config_t ifx_info;                           //Used to store the base configurations of the node's station and/or SoftAP interfaces
 tcpip_adapter_ip_info_t ipinfo;                   //Used to store the IP configurations of the node's station and/or SoftAP interfaces
 tcpip_adapter_dns_info_t dns_primary;             //Used to store the primary DNS servers of the node's station and/or SoftAP interfaces
 tcpip_adapter_dns_info_t dns_secondary;           //Used to store the secondary DNS server of node's the station interface (note the SoftAP interface currently doesn't support a secondary DNS server set)
 wifi_ps_type_t sta_ps_mode;                       //Used to store the node's station's power saving mode
 char sta_ps_str[8];                               //Used to print the node's station power saving mode as a string
 char softap_auth_str[14];                         //Used to print the node's SoftAP authmode as a string

 vTaskDelay(2000/portTICK_PERIOD_MS);              //Initial delay (for a cleaner output during the execution)

 //Logging loop (that runs while the node is connected to a parent)
 while(((eventbits = xEventGroupGetBits(mesh_event_group))>>1)&1)
  {
   ESP_ERROR_CHECK(esp_wifi_get_mac(ESP_IF_WIFI_STA,node_mac));     //Retrieve the node's (station) MAC address
   ESP_ERROR_CHECK(esp_wifi_ap_get_sta_list(&children));            //Retrieve information on the connected children nodes
   typeToStr(mesh_state.my_type,node_type_str);                     //Convert the node's type to string
   orgToStr(mesh_state.org,mesh_org_str);                           //Convert the node's mesh organization mode to string
 
   ESP_LOGI(TAG,"================================= [Logging Driver] =================================");
   ESP_LOGI(TAG,"Node MAC = "MACSTR", Node Type = %s, Node Layer = %i",MAC2STR(node_mac),node_type_str,mesh_state.my_layer);
   ESP_LOGI(TAG,"Mesh ID = "MACSTR", Mesh Wi-Fi Channel = %u, Mesh Organization Mode = %s",MAC2STR(mesh_state.mid),mesh_state.channel,mesh_org_str);
   ESP_LOGI(TAG,"Root Address = "MACSTR", Parent Address = "MACSTR,MAC2STR(mesh_state.root_addr),MAC2STR(mesh_state.parent_addr));
   ESP_LOGI(TAG,"Connected Children Nodes = %i (max = %i), Total Nodes in the Mesh Network = %i (max = %i)",children.num,esp_mesh_get_ap_connections(),esp_mesh_get_total_node_num(),esp_mesh_get_capacity_num());
   if(esp_mesh_get_group_num())                                     //If the node belongs to a mesh group, print its GID
    {
     ESP_ERROR_CHECK(esp_mesh_get_group_list((mesh_addr_t*)node_group,1));
     ESP_LOGI(TAG,"Node added into Mesh Group with GID = "MACSTR,MAC2STR(node_group));
    }
   else
    ESP_LOGI(TAG,"Node not added into any Mesh Group");

   if(MESH_DRIVER_LOGGING_ROUTING)                                        //Whether to print the node's routing table
    {
     rtable_entries = esp_mesh_get_routing_table_size();                  //Retrieve the number of entries in the node's routing table
     rtable = (mesh_addr_t*)malloc(rtable_entries*sizeof(mesh_addr_t));    //Dynamically allocate the memory required to store the node's routing table
     ESP_ERROR_CHECK(esp_mesh_get_routing_table(rtable,rtable_entries*6,&rtable_entries)); //Retrieve the node's routing table
     
     printf("\n");
     ESP_LOGI(TAG,"    Routing Table (size = %i)",rtable_entries);  //Print the node's routing table, 
     ESP_LOGI(TAG,"o===============================o");
     ESP_LOGI(TAG,"| 1 - "MACSTR" (self)  |",MAC2STR(rtable->addr)); //The first entry in a node's routing table is always relative to its (station) MAC address
     for(int i=1;i<rtable_entries;i++)
      {
       if(checkIfChild((rtable+i)->addr,&children))                  //Print the other entries of the node's routing table specifying if they refer to a child or a descendant of the node
        ESP_LOGI(TAG,"| %i - "MACSTR" (child) |",i+1,MAC2STR((rtable+i)->addr));
       else
        ESP_LOGI(TAG,"| %i - "MACSTR" (desc)  |",i+1,MAC2STR((rtable+i)->addr));
      }
     ESP_LOGI(TAG,"o===============================o");
     free(rtable);                                                  //Release the dynamic memory used to store the node's routing table
    }                          
                                          
   if(MESH_DRIVER_LOGGING_STATION)                                  //Whether to print information on the node's station interface
    {                                                       
     ESP_ERROR_CHECK(esp_wifi_get_mac(ESP_IF_WIFI_STA,mac));                                               //Retrieve the node's station interface MAC address
     ESP_ERROR_CHECK(esp_wifi_get_config(ESP_IF_WIFI_STA,&ifx_info));                                      //Retrieve the node's station interface base configuration
     ESP_ERROR_CHECK(esp_wifi_get_ps(&sta_ps_mode));                                                       //Retrieve the node's station interface power saving mode
     ESP_ERROR_CHECK(tcpip_adapter_get_ip_info(ESP_IF_WIFI_STA,&ipinfo));                                  //Retrieve the node's station interface IP configuration
     ESP_ERROR_CHECK(tcpip_adapter_get_dns_info(ESP_IF_WIFI_STA,TCPIP_ADAPTER_DNS_MAIN,&dns_primary));     //Retrieve the node's station interface primary DNS server address
     ESP_ERROR_CHECK(tcpip_adapter_get_dns_info(ESP_IF_WIFI_STA,TCPIP_ADAPTER_DNS_BACKUP,&dns_secondary)); //Retrieve the node's station interface secondary DNS server address
     psToStr(sta_ps_mode,sta_ps_str);                                                                      //Convert the node's station interface power saving mode to string

     printf("\n");
     ESP_LOGI(TAG,"Station Interface Configuration");
     ESP_LOGI(TAG,"------------------------------------------------------------------------------------");
     ESP_LOGI(TAG,"IP = "IPSTR", NetMask = "IPSTR", Gateway = "IPSTR,IP2STR(&ipinfo.ip),IP2STR(&ipinfo.netmask),IP2STR(&ipinfo.gw));
     ESP_LOGI(TAG,"Primary DNS = "IPSTR", Secondary DNS = "IPSTR,IP2STR(&dns_primary.ip.u_addr.ip4),IP2STR(&dns_secondary.ip.u_addr.ip4));
     ESP_LOGI(TAG,"MAC = "MACSTR", Power Saving Mode = %s",MAC2STR(mac),sta_ps_str);
     ESP_LOGI(TAG,"Connected to AP with SSID = %s, BSSID = "MACSTR,ifx_info.sta.ssid,MAC2STR(ifx_info.sta.bssid));
    }

   if(MESH_DRIVER_LOGGING_SOFTAP)                                   //Whether to print information on the node's SoftAP interface
    {
     ESP_ERROR_CHECK(esp_wifi_get_mac(ESP_IF_WIFI_AP,mac));                                                //Retrieve the node's SoftAP interface MAC address
     ESP_ERROR_CHECK(esp_wifi_get_config(ESP_IF_WIFI_AP,&ifx_info));                                       //Retrieve the node's SoftAP interface base configuration
     ESP_ERROR_CHECK(tcpip_adapter_get_ip_info(ESP_IF_WIFI_AP,&ipinfo));                                   //Retrieve the node's SoftAP interface IP configuration
     ESP_ERROR_CHECK(tcpip_adapter_get_dns_info(ESP_IF_WIFI_AP,TCPIP_ADAPTER_DNS_MAIN,&dns_primary));      //Retrieve the node's SoftAP interface primary DNS server address
     authToStr(ifx_info.ap.authmode,softap_auth_str);                                                      //Convert the node's SoftAP authmode to string

     printf("\n");
     ESP_LOGI(TAG,"SoftAP Interface Configuration");
     ESP_LOGI(TAG,"------------------------------------------------------------------------------------");
     ESP_LOGI(TAG,"SSID = %s, Password = %s, Authmode = %s, Channel = %u",ifx_info.ap.ssid,ifx_info.ap.password,softap_auth_str,ifx_info.ap.channel);
     ESP_LOGI(TAG,"IP = "IPSTR", NetMask = "IPSTR", Gateway = "IPSTR,IP2STR(&ipinfo.ip),IP2STR(&ipinfo.netmask),IP2STR(&ipinfo.gw));
     ESP_LOGI(TAG,"Primary DNS = "IPSTR", MAC = "MACSTR,IP2STR(&dns_primary.ip.u_addr.ip4),MAC2STR(mac));
    }

   if(MESH_DRIVER_LOGGING_EVENTGROUP)                               //Whether to print the flags set in the mesh event group
    {
     eventGroupSetupToStr(event_group_setup_str);                   //Convert the flags relative to the setup module in the mesh event group to string
     eventGroupDriverToStr(event_group_driver_str);                 //Convert the flags relative to the driver components executions in the mesh event group  to string

     printf("\n");
     ESP_LOGI(TAG,"Mesh Event Group");
     ESP_LOGI(TAG,"------------------------------------------------------------------------------------");
     ESP_LOGI(TAG,"Setup Flags = %s",event_group_setup_str);
     ESP_LOGI(TAG,"Driver Executions Flags = %s",event_group_driver_str);
    }

   ESP_LOGI(TAG,"====================================================================================\n");
   vTaskDelay(MESH_DRIVER_LOGGING_INTERVAL*1000/portTICK_PERIOD_MS);
  }
 ESP_LOGE(TAG,"The node is no longer connected to a parent, Logging Driver shutting down\n");
 xEventGroupClearBits(mesh_event_group,MESH_DRIVER_INTERNAL_LOGGING);
 vTaskDelete(NULL);
}

/*************************************************************************
 *              Sender Driver (MESH_DRIVER_INTERNAL_SENDER)
 *************************************************************************/
void meshSenderDriver()
{
 const static char* TAG = "[Sender_Driver]";
 EventBits_t eventbits;                                     //Used to check the flags in the mesh event group

 /*-- esp_mesh_send() data structures --*/
 esp_err_t send_ret = 0;                                    //Used to store the result of the send operation
 char send_buffer[sizeof(MESH_DRIVER_SENDER_MESSAGE)];      //Sending Buffer
 mesh_addr_t dest;                                          //Packet's Destination Address
 mesh_data_t data_des;                                      //Packet's Data Descriptor
 int send_flag = 0;                                         //Packet's Flag
 mesh_opt_t send_opt = {0};                                 //Packet's additional option (currently none is used in this example)
 uint8_t opt_val[50];                                       //Packet's additional option value (currently no additional option is used in this example)                                  

 /*-- send-to-descendants utility data structures --*/
 wifi_sta_list_t children;                                  //Used to store information on the connected children nodes
 mesh_addr_t* rtable;                                       //Used to dynamically store the node's routing table
 int rtable_entries;                                        //Used to store the number of entries in the node's routing table 

 /*-- logging data structures --*/
 char sendmode_str[15];                                     //Used to print the esp_mesh_send() sending mode as a string
 char proto_str[8];                                         //Used to print the protocol the packet's payload belongs to as string
 char tos_str[12];                                          //Used to print the packet's type of service as a string
 char flag_str[100] = {0};                                  //Used to print the bits in the packet's flag as a string
 char opt_str[100] = {0};                                   //Used to print the type and value of an additional option in the packet
 char send_err_str[28];                                     //Used to print an error in the esp_mesh_send() as a string

 //1) Copy the data to send (the payload) into the sending buffer
 memcpy(send_buffer,MESH_DRIVER_SENDER_MESSAGE,sizeof(MESH_DRIVER_SENDER_MESSAGE)); 
 
 //2) Packet Data Descriptor Inizialization
 data_des.data = (uint8_t*)send_buffer;                     //Set the address of the sending buffer
 data_des.size = sizeof(MESH_DRIVER_SENDER_MESSAGE);        //Set the size of the packet's payload
 data_des.proto = MESH_DRIVER_SENDER_PROTOCOL;              //Set the protocol the packet's payload belongs to
 data_des.tos = MESH_DRIVER_SENDER_TOS;                     //Set the type of service that must be used to deliver the packet

 //3) Send Flag Initialization
 if(MESH_DRIVER_SENDER_MODE == SEND_NONBLOCK)               //Whether to use the blocking or the non-blocking version of the esp_mesh_send() to send the packet
  send_flag += MESH_DATA_NONBLOCK;                          /** TODO BUG: Currently using the non-blocking mode still causes the esp_mesh_send() function to block if it would block in the first place  */
                                                            /**           (it should also be noted that the Mesh API currently don't offer any method to set a timeout for the esp_mesh_send() function) */
 //4) Destination-specific Initializations
 switch(MESH_DRIVER_SENDER_DEST)
  {
   case SEND_PARENT:
    send_flag += MESH_DATA_P2P;                             //Since the parent of a node is (in general) a non-root node, the MESH_DATA_P2P bit must be set in the packet's flag
    memcpy(dest.addr,mesh_state.parent_addr,6);             //Set the packet's destination address to the MAC address of the node's parent 
    break;                                                  //(note that this represents the only case where it's safe to use a node's SoftAP MAC address as a packet's destination  
   case SEND_FIRSTCHILD:                                    //address, since in general it is the station MAC addresses of nodes that is propagated upwards in the routing table of  
   case SEND_ALLCHILDREN:                                   //their ancestors, and so represent the label for which nodes are addressable in a mesh network)
   case SEND_ALLDESCENDANTS:
    send_flag += MESH_DATA_P2P;                             //Since a node's descendants are non-root nodes, the MESH_DATA_P2P bit must be set in the packet's flag
    break;                                                  //Since a node's descendants may vary during the program's execution, their MAC addresses are retrieved dynamically in the main sending loop
   case SEND_ROOT:                                          //Since when sending to the root node the "dest" argument of the esp_mesh_send() function must be set to NULL it's not necessary to set the 
    break;                                                  //packet's destination address and note that in this case the mesh stack will autonomously set the MESH_DATA_TODS bit in the packet's flag
   case SEND_SPECIFIC:                  
    send_flag += MESH_DATA_P2P;                             //Since a specific node's is (in general) a non-root node, the MESH_DATA_P2P bit must be set in the packet's flag
    strToMAC(MESH_DRIVER_SENDER_SPECIFIC_ADDR,dest.addr);   //Set the packet's destination address to the specific node's MAC
    break;
   case SEND_GROUP:
    send_flag += MESH_DATA_P2P;                             //Since a mesh group is internal to the mesh network, the MESH_DATA_P2P bit must be set in the packet's flag
    send_flag += MESH_DATA_GROUP;                           //When sending to a mesh group the MESH_DATA_GROUP bit must be set in the packet's flag
    strToMAC(MESH_DRIVER_SENDER_DEST_GROUP_ADDR,dest.addr); //Set the packet's destination address to the GID of the mesh group
    break;
   case SEND_BROADCAST:                                     //Set the packet's destination to the broadcast address (FF:FF:FF:FF:FF)
    memset(dest.addr,1,sizeof(dest.addr));                  /** TODO BUG: Currently sending a mesh packet in broadcast causes the esp_mesh_send() function to return the ESP_ERR_MESH_NO_ROUTE_FOUND error */
    break;
  }

 //Convert the packet's parameters to string for logging
 sendModeToStr(MESH_DRIVER_SENDER_MODE,sendmode_str);       //Convert the packet's sending mode to string
 proToStr(data_des.proto,proto_str);                        //Convert the protocol the packet's payload belongs to to string
 tosToStr(data_des.tos,tos_str);                            //Convert the packet's type of service to string
 commFlagToStr(send_flag,flag_str);                         //Convert the bits in the packet's flag to string
 commOptToStr(&send_opt,opt_val,opt_str);                   //Convert the type and value of an eventual additional option in the packet to string (currently none is used in this example)
     
 vTaskDelay(3000/portTICK_PERIOD_MS);                       //Initial delay (for a cleaner output during the execution)

 //5) Sending loop with destination-specific send() calls (that runs while the node is connected to a parent) 
 while(((eventbits = xEventGroupGetBits(mesh_event_group))>>1)&1)
  {
   ESP_LOGI(TAG,"================================== [Sender Driver] ==================================");
   switch(MESH_DRIVER_SENDER_DEST)
    {
     case SEND_PARENT:
      if(mesh_state.my_type == MESH_ROOT)                                                     //If the node is root, sending mesh packets to its parent (the router) has no sense
       {
        ESP_LOGI(TAG,"Since the node is root, sending mesh packets to its parent (the router) has no sense ");
        ESP_LOGI(TAG,"Sender Driver shutting down");
        ESP_LOGI(TAG,"=====================================================================================\n");
        xEventGroupClearBits(mesh_event_group,MESH_DRIVER_INTERNAL_SENDER);
        vTaskDelete(NULL);
       }
      if(!(send_ret = esp_mesh_send(&dest,&data_des,send_flag,NULL,0)))                       //Send the packet to the node's parent
       ESP_LOGI(TAG,"Message \"%s\" successfully sent to the node's parent with MAC = "MACSTR,send_buffer,MAC2STR(dest.addr));
      break;
 
     case SEND_FIRSTCHILD:
      waitForChildren();                                                                      //If sending to the node's first child, wait for it to connect (MESH_CHILD) before resuming the sender driver
      ESP_ERROR_CHECK(esp_wifi_ap_get_sta_list(&children));                                   //Retrieve the node's first child station MAC address and set it as the packet's destination address
      memcpy(dest.addr,children.sta[0].mac,6);                                                               
      if(!(send_ret = esp_mesh_send(&dest,&data_des,send_flag,NULL,0)))                       //Send the packet to the node's first child
       ESP_LOGI(TAG,"Message \"%s\" successfully sent to the node's first child with MAC = "MACSTR,send_buffer,MAC2STR(dest.addr));
      break;
      
     case SEND_ALLCHILDREN:
      waitForChildren();                                                                      //If sending to all the node's children, wait for at least one of them to connect (MESH_CHILD) before resuming the sender driver 
      ESP_ERROR_CHECK(esp_wifi_ap_get_sta_list(&children));                                   //Retrieve the node's children station MAC addresses and send the packet to each of them
      for(int i=0;i<children.num;i++)                                                                        
       {
        memcpy(dest.addr,children.sta[i].mac,6);
        if((send_ret = esp_mesh_send(&dest,&data_des,send_flag,NULL,0)))                      //Send the packet to all the node's children (where if a send() returns an error, exit from the loop to print it)
         break;
       }
      if(!send_ret)
       ESP_LOGI(TAG,"Message \"%s\" successfully sent to all the node's children (%i)",send_buffer,children.num);
      break;

     case SEND_ALLDESCENDANTS: 
      waitForChildren();                                                                      //If sending to all the node's descendants, wait for at least one child to connect (MESH_CHILD) before resuming the sender driver
      rtable_entries = esp_mesh_get_routing_table_size();                                     //Retrieve the number of entries in the node's routing table
      rtable = (mesh_addr_t*)malloc(rtable_entries*sizeof(mesh_addr_t));                      //Dynamically allocate the memory required to store the node's routing table
      ESP_ERROR_CHECK(esp_mesh_get_routing_table(rtable,rtable_entries*6,&rtable_entries));   //Retrieve the node's routing table
      for(int i=1;i<rtable_entries;i++)                                                       //Skip the first entry in the node's routing table, which is always relative to its own station MAC address                  
       {
        memcpy(dest.addr,(rtable+i)->addr,6);
        if((send_ret = esp_mesh_send(&dest,&data_des,send_flag,NULL,0)))                      //Send the packet to all the node's descendants (where if a send() returns an error, exit from the loop to print it)
         break;
       }
      free(rtable);                                                                           //Release the dynamic memory used to store the node's routing table       
      if(!send_ret) 
       ESP_LOGI(TAG,"Message \"%s\" successfully sent to all the node's descendants (%i)",send_buffer,rtable_entries-1);
      break;

     case SEND_ROOT:                                                                          //If sending to the root node, the "dest" argument of the esp_mesh_send() function must be set to NULL
      if(!(send_ret = esp_mesh_send(NULL,&data_des,send_flag,NULL,0)))                        //Send the packet to the root node
       ESP_LOGI(TAG,"Message \"%s\" successfully sent to the root node with MAC = "MACSTR,send_buffer,MAC2STR(mesh_state.root_addr));
      break;
 
     case SEND_SPECIFIC:
      if(!(send_ret = esp_mesh_send(&dest,&data_des,send_flag,NULL,0)))                       //Send the packet to the specific node
       ESP_LOGI(TAG,"Message \"%s\" successfully sent to the specific node with MAC = "MACSTR,send_buffer,MAC2STR(dest.addr));
      break;
 
     case SEND_GROUP:
      if(!(send_ret = esp_mesh_send(&dest,&data_des,send_flag,NULL,0)))                       //Send the packet to the mesh group
       ESP_LOGI(TAG,"Message \"%s\" successfully sent to the mesh group with GID = "MACSTR,send_buffer,MAC2STR(dest.addr));
      break;
     
     case SEND_BROADCAST:                                                   /** TODO BUG: Currently sending a mesh packet in broadcast causes the esp_mesh_send() function to return the ESP_ERR_MESH_NO_ROUTE_FOUND error */
      if(!(send_ret = esp_mesh_send(&dest,&data_des,send_flag,NULL,0)))                       //Send the packet in broadcast to all nodes in the mesh network
       ESP_LOGI(TAG,"Message \"%s\" successfully sent in broadcast to all nodes in the mesh network",send_buffer);
      break;

     default:
      ESP_LOGE(TAG,"Unknown sending destination, Sender Driver shutting down");
      xEventGroupClearBits(mesh_event_group,MESH_DRIVER_INTERNAL_SENDER);
      vTaskDelete(NULL);
      break;
    }
  
   //6) Check and log the result of the send operation
   if(send_ret)                                                                               //If the esp_mesh_send() returned an error, print it along with the packet's parameters
    {
     commErrToStr(send_ret,send_err_str);                                              
     ESP_LOGE(TAG,"send(): %s",send_err_str);
     ESP_LOGE(TAG,"Message = \"%s\", Packet size = %i, Destination = "MACSTR,send_buffer,strlen(send_buffer)+1,MAC2STR(dest.addr));
     ESP_LOGE(TAG,"Sending mode = %s, Payload protocol = %s, Packet TOS = %s",sendmode_str,proto_str,tos_str);
     ESP_LOGE(TAG,"Flag = %s, Additional Option = %s",flag_str,opt_str);
    }
   else                                                                                       //Otherwise if the esp_mesh_send() was successful, just print the packet's parameters
    {
     ESP_LOGI(TAG,"Packet size = %i, Sending mode = %s, Payload protocol = %s, Packet TOS = %s",strlen(send_buffer)+1,sendmode_str,proto_str,tos_str);
     ESP_LOGI(TAG,"Flag = %s, Additional Option = %s",flag_str,opt_str);
    }                                                                                                      
   ESP_LOGI(TAG,"=====================================================================================\n");
   vTaskDelay(MESH_DRIVER_SENDER_INTERVAL*1000/portTICK_PERIOD_MS);
  }
 ESP_LOGE(TAG,"The node is no longer connected to a parent, Sender Driver shutting down");
 xEventGroupClearBits(mesh_event_group,MESH_DRIVER_INTERNAL_SENDER);
 vTaskDelete(NULL);
}

/*************************************************************************
 *            Receiver Driver (MESH_DRIVER_INTERNAL_RECEIVER)
 *************************************************************************/
void meshReceiverDriver()
{
 const static char* TAG = "[Receiver_Driver]";
 EventBits_t eventbits;                               //Used to check the flags in the mesh event group

 /*-- esp_mesh_recv() data structures --*/
 esp_err_t recv_ret = 0;                              //Used to store the result of the receive operation
 char recv_buffer[MESH_MTU];                          //Receiving Buffer (MESH_MTU = 1500 bytes)
 mesh_addr_t src = {0};                               //Used to store the received packet's source address 
 mesh_data_t data_des = {0};                          //Used to store the received packet's data descriptor
 int recv_flag = 0;                                   //Used to store the received packet's flag
 mesh_opt_t recv_opt = {0};                           //Used to store the received packet's additional option
 uint8_t opt_val[50];                                 //Used to store the received packet's additional option's value   
 bool fromDS;                                         //Denotes whether the data in a received packet originated from an external host or from a node inside the mesh network
 ip4_addr_t exthost_ip;                               //Used to store the IP address of the external host the data in the received packet originated from
 uint16_t exthost_port;                               //Used to store the port of the external host the data in the received packet originated from

 /*-- logging data structures --*/
 char sendmode_str[15] = {0};                         //Used to print the reply sending mode as a string
 char proto_str[8] = {0};                             //Used to print the protocol the payload of the received packet belongs to as a string
 char tos_str[12] = {0};                              //Used to print the received packet's type of service as a string
 char flag_str[100] = {0};                            //Used to print the bits of the received packet's flag as a string
 char opt_str[100] = {0};                             //Used to print the received packet's additional option type and value as a string
 char recv_err_str[28] = {0};                         //Used to print an error in the esp_mesh_recv() as a string
 
 //1) Set the address of the receive buffer
 data_des.data = (uint8_t*)recv_buffer;

 //2) Set the address of the buffer where to store the value of an additional option in the received packet
 recv_opt.val = opt_val;

 //Main Receiving Loop (that runs while the node is connected to a parent)
 while(((eventbits = xEventGroupGetBits(mesh_event_group))>>1)&1)
  {
   //3) Reset the "size" attribute of the packet data descriptor to the data length in bytes that can be received at the application-level, i.e. the size of the receiving buffer
   data_des.size = MESH_MTU;                                                                              
   
   //4) Reset the additional option received in the previous packet, if any                                                                                                       
   recv_opt.type = 0;

   //5) Receive the Packet
   recv_ret = esp_mesh_recv(&src,&data_des,portMAX_DELAY,&recv_flag,&recv_opt,1);  //Blocking receive

   //6) Check and log the result of the receive operation
   ESP_LOGI(TAG,"================================ [Receiver Driver] ================================");
   if(recv_ret)                                                         //If the esp_mesh_recv() returned an error, convert it to string and print it
    {
     commErrToStr(recv_ret,recv_err_str);                                              
     ESP_LOGE(TAG,"recv(): %s",recv_err_str);
    }
   else                                                                 //Otherwise if the esp_mesh_recv() returned successfully, print the received packet's message and parameters
    {
     //Check whether the data in the packet originated from an external host or from a node inside the mesh network
     if(((recv_flag>>3)&1)&&(recv_opt.type == MESH_OPT_RECV_DS_ADDR))   //If both the MESH_DATA_FROMDS bit in the flag and the MESH_OPT_RECV_DS_ADDR additional option are set in the  
      {                                                                 //received packet its data originated from an external host, otherwise from another node inside the mesh network
       fromDS = true;                                                   //If the data in the packet originated from an external host, retrieve its IP 
       memcpy(&exthost_ip,opt_val,4);                                   //address and port from the value of the MESH_OPT_RECV_DS_ADDR additional option     
       memcpy(&exthost_port,opt_val+4,2);
      }
     else
      fromDS = false;                                                                                                      
 
     //Convert the received packet's parameters to string for logging
     proToStr(data_des.proto,proto_str);                                 //Convert the protocol the received packet's payload belongs to to string
     tosToStr(data_des.tos,tos_str);                                     //Convert the received packet's type of service to string
     commFlagToStr(recv_flag,flag_str);                                  //Convert the bits in the received packet's flag to string
     commOptToStr(&recv_opt,opt_val,opt_str);                            //Convert the type and value of an eventual additional option in the received packet to string
     recv_buffer[data_des.size] = '\0';                                  //Ensure that the received message can be printed as a string

     //Print the received packet's message and parameters
     if(fromDS)
      ESP_LOGI(TAG,"Received Data = \"%s\" from external host "IPSTR":%u",recv_buffer,IP2STR(&exthost_ip),exthost_port);
     else
      {
       if(MESH_DRIVER_RECEIVER_REPLY&&(!strcmp(MESH_DRIVER_RECEIVER_REPLY_MESSAGE,recv_buffer)))   //If the received message represents a reply to a previous message sent by this node 
        ESP_LOGI(TAG,"Received reply \"%s\" from address = "MACSTR,recv_buffer,MAC2STR(src.addr));
       else
        ESP_LOGI(TAG,"Received Message = \"%s\" from address = "MACSTR,recv_buffer,MAC2STR(src.addr));
      }
     ESP_LOGI(TAG,"Packet size = %i, Payload protocol = %s, Packet TOS = %s",strlen(recv_buffer)+1,proto_str,tos_str);
     ESP_LOGI(TAG,"Flag = %s, Additional Option = %s",flag_str,opt_str);
    
     //If the node should reply a predefined message to the source of the received packet (only for sources inside the mesh network and if the received message differs from the predefined reply to avoid loops)
     if(MESH_DRIVER_RECEIVER_REPLY&&(!fromDS)&&(strcmp(MESH_DRIVER_RECEIVER_REPLY_MESSAGE,recv_buffer)))                                             
      {                                         
       /* Note that to save on memory resources the reply packet is sent reusing the same data structures used for receiving the
          packet (this obviously must be avoided if further processing on the received packet is desidered at the application level) */
                                                       
       //1) Copy the reply message (the payload) into the sending buffer   
       memcpy(recv_buffer,MESH_DRIVER_RECEIVER_REPLY_MESSAGE,sizeof(MESH_DRIVER_RECEIVER_REPLY_MESSAGE));   
 
       //2) Packet Data Descriptor Inizialization
       data_des.size = sizeof(MESH_DRIVER_RECEIVER_REPLY_MESSAGE);       //Set the size of the reply packet's payload
       data_des.proto = MESH_DRIVER_RECEIVER_REPLY_PROTOCOL;             //Set the protocol the reply packet's payload belongs to
       data_des.tos = MESH_DRIVER_RECEIVER_REPLY_TOS;                    //Set the type of service that must be used to deliver the reply packet

       //3) Send Flag Initialization
       recv_flag = 0;                                                    //Since the received packet's flag is reused to send the reply packet, reset it
       if(MESH_DRIVER_RECEIVER_REPLY_MODE == SEND_NONBLOCK)              //Whether to use the blocking or the non-blocking version of the esp_mesh_send() for the reply
        recv_flag += MESH_DATA_NONBLOCK;                                 /** TODO BUG: Currently using the non-blocking mode still causes the esp_mesh_send() function to block if it would block in the first place  */
                                                                         /**           (it should also be noted that the Mesh API currently don't offer any method to set a timeout for the esp_mesh_send() function) */
       //4) Destination-specific Initializations
       recv_flag += MESH_DATA_P2P;                                       //Since the reply packet's destination is (in general) a non-root node, the MESH_DATA_P2P bit must be set	 
       recv_opt.type = 0;                                                //Since the received packet's additional option is reused to send the reply packet, reset it
       
       //Convert the sending parameters to string for logging
       sendModeToStr(MESH_DRIVER_RECEIVER_REPLY_MODE,sendmode_str);      //Convert the reply packet's sending mode to string
       proToStr(data_des.proto,proto_str);                               //Convert the protocol the reply packet's payload belongs to to string
       tosToStr(data_des.tos,tos_str);                                   //Convert the reply packet's type of service to string
       commFlagToStr(recv_flag,flag_str);                                //Convert the bits in the flag of the reply packet to string
       commOptToStr(&recv_opt,opt_val,opt_str);                          //Converts the type and value of an eventual additional option in the reply packet to string (currently none is used in this example)

       //5) Destination-specific send() call
       ESP_LOGI(TAG,"-----------------------------------------------------------------------------------");
       if((recv_ret = esp_mesh_send(&src,&data_des,recv_flag,NULL,0)))   //If the esp_mesh_send() returned an error, print it along with the reply packet's parameters
        {
         commErrToStr(recv_ret,recv_err_str);                                              
         ESP_LOGE(TAG,"reply send(): %s",recv_err_str);
         ESP_LOGE(TAG,"Message = \"%s\", Packet size = %i, Destination = "MACSTR,recv_buffer,strlen(recv_buffer)+1,MAC2STR(src.addr));
         ESP_LOGE(TAG,"Sending mode = %s, Payload protocol = %s, Packet TOS = %s",sendmode_str,proto_str,tos_str);
         ESP_LOGE(TAG,"Flag = %s, Additional Option = %s",flag_str,opt_str);
        }
       else                                                              //Otherwise if the esp_mesh_send() was successful, just print the packet's parameters
        {
         ESP_LOGI(TAG,"Replied \"%s\" to the sender",recv_buffer);
         ESP_LOGI(TAG,"Packet size = %i, Payload Protocol = %s, Packet TOS = %s",strlen(recv_buffer)+1,proto_str,tos_str);
         ESP_LOGI(TAG,"Flag = %s, Additional Option = %s",flag_str,opt_str);
        }
      }
    } 
   ESP_LOGI(TAG,"===================================================================================\n");
  }
 ESP_LOGE(TAG,"The node is no longer connected to a parent, Receiver Driver shutting down");
 xEventGroupClearBits(mesh_event_group,MESH_DRIVER_INTERNAL_RECEIVER);
 vTaskDelete(NULL);
}

/*************************************************************************
 *           SenderDS Driver (MESH_DRIVER_EXTERNAL_SENDERDS)
 *************************************************************************/
void meshSenderDSDriver()
{
 const static char* TAG = "[SenderDS_Driver]";
 EventBits_t eventbits;                                     //Used to check the flags in the mesh event group

 /*-- esp_mesh_send() data structures --*/
 esp_err_t send_ret = 0;                                    //Used to store the result of the send operation
 char send_buffer[sizeof(MESH_DRIVER_SENDERDS_MESSAGE)];    //Sending Buffer
 mesh_addr_t dest;                                          //Packet's Destination Address
 mesh_data_t data_des;                                      //Packet's Data Descriptor
 int send_flag = 0;                                         //Packet's Flag
 mesh_opt_t send_opt = {0};                                 //Packet's additional option (none since currently no additional option is involved or of use in sending a packets towards an external host)
 uint8_t opt_val[50];                                       //Used to store the value of the packet's additional option (currently of no use, see above)                                 

 /*-- logging data structures --*/
 char sendmode_str[15];                                     //Used to print the esp_mesh_send() sending mode as a string
 char proto_str[8];                                         //Used to print the protocol the packet's payload belongs to as string
 char tos_str[12];                                          //Used to print the packet's type of service as a string
 char flag_str[100] = {0};                                  //Used to print the bits in the packet's flag as a string
 char opt_str[100] = {0};                                   //Used to print the type and value of an additional option in the packet
 char send_err_str[28];                                     //Used to print an error in the esp_mesh_send() as a string

 //1) Copy the data to send (the payload) into the sending buffer
 memcpy(send_buffer,MESH_DRIVER_SENDERDS_MESSAGE,sizeof(MESH_DRIVER_SENDERDS_MESSAGE)); 
 
 //2) Packet Data Descriptor Inizialization
 data_des.data = (uint8_t*)send_buffer;                     //Set the address of the sending buffer
 data_des.size = sizeof(MESH_DRIVER_SENDERDS_MESSAGE);      //Set the size of the message to send
 data_des.proto = MESH_DRIVER_SENDERDS_PROTOCOL;            //Set the protocol the packet's payload belongs to
 data_des.tos = MESH_DRIVER_SENDERDS_TOS;                   //Set the type of service that must be used to deliver the packet

 //3) Send Flag Initializations
 if(MESH_DRIVER_SENDER_MODE == SEND_NONBLOCK)               //Whether to use the blocking or the non-blocking version of the esp_mesh_send() to send the packet
  send_flag += MESH_DATA_NONBLOCK;                          /** TODO BUG: Currently using the non-blocking mode still causes the esp_mesh_send() function to block if it would block in the first place  */
                                                            /**           (it should also be noted that the Mesh API currently don't offer any method to set a timeout for the esp_mesh_send() function) */ 
 if(MESH_DRIVER_SENDERDS_DROP)                              //Whether to mark the data in the packet as of no critical importance to allow it to be dropped in case of congestion on a newly elected root node
  send_flag += MESH_DATA_DROP;                              /** TODO BUG: Currently the MESH_DATA_DROP bit in a packet's flag is either not actually set in the  */
                                                            /**           sent packet or in any case not received at the application level on the root node      */                       
 //4) Destination-specific Initializations
 send_flag += MESH_DATA_TODS;                               //Since this driver component is used exclusively to send data towards an external host, the MESH_DATA_TODS bit must be set in the packet's flag 
 inet_pton(AF_INET,MESH_DRIVER_SENDERDS_DESTINATION_IP,&dest.mip.ip4);  //Set the destination IP address
 dest.mip.port = MESH_DRIVER_SENDERDS_DESTINATION_PORT;                 //Set the destination port

 //Convert the packet's parameters to string for logging
 sendModeToStr(MESH_DRIVER_SENDERDS_MODE,sendmode_str);     //Convert the packet's sending mode to string
 proToStr(data_des.proto,proto_str);                        //Convert the protocol the packet's payload belongs to to string
 tosToStr(data_des.tos,tos_str);                            //Convert the packet's type of service to string
 commFlagToStr(send_flag,flag_str);                         //Convert the bits in the packet's flag to string
 commOptToStr(&send_opt,opt_val,opt_str);                   //Convert the type and value of an eventual additional option in the packet to string (currently none is used in this example)

 //5) Sending loop with destination-specific send() call (that runs while the external DS is accessible)
 while(((eventbits = xEventGroupGetBits(mesh_event_group))>>3)&1)
  {
   ESP_LOGI(TAG,"=================================[SenderDS Driver]=================================");
   waitForNoVote();                                             //While a new root election is in progress, a node should refrain from transmitting towards the external DS
   send_ret = esp_mesh_send(&dest,&data_des,send_flag,NULL,0);  //Send the packet towards an external host
 
   //6) Check and log the result of the send operation  
   if(send_ret)                                                 //If the esp_mesh_send() returned an error, print it along with the packet's parameters
    {
     commErrToStr(send_ret,send_err_str);                                              
     ESP_LOGE(TAG,"send(): %s",send_err_str);
     ESP_LOGE(TAG,"Message = \"%s\", Packet size = %i, Destination = "IPSTR":%u",send_buffer,strlen(send_buffer)+1,IP2STR(&dest.mip.ip4),dest.mip.port);
     ESP_LOGE(TAG,"Sending mode = %s, Payload Protocol = %s, Packet TOS = %s",sendmode_str,proto_str,tos_str);
     ESP_LOGE(TAG,"Flag = %s, Additional Option = %s",flag_str,opt_str);
    }
   else                                                         //Otherwise if the esp_mesh_send() was successful, just print the packet's parameters
    {
     ESP_LOGI(TAG,"Message = \"%s\" successfully sent towards the external host "IPSTR":%u",send_buffer,IP2STR(&dest.mip.ip4),dest.mip.port);
     ESP_LOGI(TAG,"Packet size = %i, Sending mode = %s, Payload Protocol = %s, Packet TOS = %s",strlen(send_buffer)+1,sendmode_str,proto_str,tos_str);
     ESP_LOGI(TAG,"Flag = %s, Additional Option = %s",flag_str,opt_str);
    }
   ESP_LOGI(TAG,"===================================================================================\n");
   vTaskDelay(MESH_DRIVER_SENDERDS_INTERVAL*1000/portTICK_PERIOD_MS);
  }
 ESP_LOGE(TAG,"The external DS is no longer accessible, SenderDS Driver shutting down");
 xEventGroupClearBits(mesh_event_group,MESH_DRIVER_EXTERNAL_SENDERDS);
 vTaskDelete(NULL);
}

/*************************************************************************
 *            ReceiverDS Driver (MESH_DRIVER_ROOT_RECEIVERDS)
 *************************************************************************/
void meshReceiverDSDriver()
{
 const static char* TAG = "[ReceiverDS_Driver]";

 /*-- esp_mesh_recv() data structures --*/
 esp_err_t recv_ret = 0;                              //Used to store the result of the receive operation
 char recv_buffer[MESH_MTU];                          //Receiving Buffer (MESH_MTU = 1500 bytes)
 mesh_addr_t src = {0};                               //Used to store the received packet's source address
 mesh_addr_t dest = {0};                              //Used to store the received packet's external destination (IPv4:PORT)
 mesh_data_t data_des = {0};                          //Used to store the received packet's data descriptor
 int recv_flag = 0;                                   //Used to store the received packet's flag
 mesh_opt_t recv_opt = {0};                           //Used to store the received packet's additional option (note that currently no additional option is involved or of use in sending a packets towards an external host)
 uint8_t opt_val[50];                                 //Used to store the received packet's additional option's value (see above)                                

 /*-- logging data structures --*/
 char proto_str[8] = {0};                             //Used to print the protocol the payload of the received packet belongs to as a string
 char tos_str[12] = {0};                              //Used to print the received packet's type of service as a string
 char flag_str[100] = {0};                            //Used to print the bits of the received packet's flag as a string
 char opt_str[100] = {0};                             //Used to print the received packet's additional option type and value as a string
 char recv_err_str[28] = {0};                         //Used to print an error in the esp_mesh_recv() as a string
 
 //1) Set the address of the receive buffer
 data_des.data = (uint8_t*)recv_buffer;

 //2) Set the address of the buffer where to store the value of an additional option in the received packet
 recv_opt.val = opt_val;

 //Main ReceivingDS Loop (that runs while the node is root)
 while(mesh_state.my_type == MESH_ROOT)
  {
   //3) Reset the "size" attribute of the packet data descriptor to the data length in bytes that can be received at the application-level, i.e. the size of the receiving buffer
   data_des.size = MESH_MTU;                                                                              
   
   //4) Reset the additional option received in the previous packet, if any                                                                                                       
   recv_opt.type = 0;

   //5) Receive the packet whose data is destined for an external host
   recv_ret = esp_mesh_recv_toDS(&src,&dest,&data_des,portMAX_DELAY,&recv_flag,&recv_opt,1);  //Blocking receive

   //6) Check and log the result of the receive operation
   ESP_LOGI(TAG,"============================== [ReceiverDS Driver] ==============================");
   if(recv_ret)                                                                                            //If the esp_mesh_recv_toDS() returned an error, convert it to string and print it
    {
     commErrToStr(recv_ret,recv_err_str);                                              
     ESP_LOGE(TAG,"recvDS(): %s",recv_err_str);
    }
   else                                                                                                    //Otherwise if the esp_mesh_recv_toDS() returned successfully, print the received packet's message and parameters
    {                                                                                                      
     //Convert the received packet's parameters to string for logging
     proToStr(data_des.proto,proto_str);                                 //Convert the protocol the received packet's payload belongs to to string
     tosToStr(data_des.tos,tos_str);                                     //Convert the received packet's type of service to string
     commFlagToStr(recv_flag,flag_str);                                  //Convert the bits in the received packet's flag to string
     commOptToStr(&recv_opt,opt_val,opt_str);                            //Convert the type and value of an eventual additional option in the received packet to string
     recv_buffer[data_des.size] = '\0';                                  //Ensure that the received message can be printed as a string
     
     //Print the received packet's message and parameters
     ESP_LOGI(TAG,"Received Packet from "MACSTR" destined for external host "IPSTR":%u",MAC2STR(src.addr),IP2STR(&dest.mip.ip4),dest.mip.port);
     ESP_LOGI(TAG,"Packet payload = \"%s\"",recv_buffer);
     ESP_LOGI(TAG,"Packet size = %i, Payload protocol = %s, Packet TOS = %s",strlen(recv_buffer)+1,proto_str,tos_str);
     ESP_LOGI(TAG,"Flag = %s, Additional Option = %s",flag_str,opt_str);

     /* =============================================================================================================================
        |From here by using IP sockets and possibly a Port Address Translation (PAT) mechanism, the root node can forward data to   |
        |the external router and establish connections with remote hosts on behalf of the nodes in the mesh network (not shown here)|      
        ============================================================================================================================= */
    }
   ESP_LOGI(TAG,"=================================================================================\n");
  }
 ESP_LOGE(TAG,"The node is no longer root, ReceiverDS Driver shutting down");
 xEventGroupClearBits(mesh_event_group,MESH_DRIVER_ROOT_RECEIVERDS);
 vTaskDelete(NULL);
}

/*************************************************************************
 *                Waive Driver (MESH_DRIVER_ROOT_WAIVE)
 *************************************************************************/
void meshWaiveDriver()
{
 const static char* TAG = "[Waive_Driver]";
 EventBits_t eventbits;                        //Used to check the flags in the mesh event group
 mesh_vote_t vote_settings;                    //Used to store the new root election settings
 float newroot_voting_threshold;               //Used to store the new root election voting percentage threshold
 wifi_ap_record_t routerAP;                    //Used to store information on the Router's AP the root station interface is connected to                                   

 //Initialize the new root election settings
 vote_settings.is_rc_specified = false;                                  //Whether the root node appoints directly another node as the new root instead of triggering a new root election (currently unimplemented)
 vote_settings.config.attempts = MESH_DRIVER_WAIVE_ELECTION_MAX_ROUNDS;  //Set the maximum number of rounds in the new root election
 newroot_voting_threshold = atof(MESH_DRIVER_WAIVE_VOTING_THRESHOLD);    //Convert the new root election voting percentage threshold menuconfig string to float
 if((newroot_voting_threshold<=0.0)||(newroot_voting_threshold>1.0))     //If the new root election voting percentage threshold is out of bounds, abort the Waive Driver
  {
   ESP_LOGE(TAG,"Invalid new root election voting percentage threshold \"%f\", aborting the Waive Driver",newroot_voting_threshold);
   xEventGroupClearBits(mesh_event_group,MESH_DRIVER_ROOT_WAIVE);        
   vTaskDelete(NULL);
  }
 else
  vote_settings.percentage = newroot_voting_threshold;  //Set the new root election voting percentage threshold
   
 //Whether the root waive and so the new root election should be triggered by the root's RSSI with the router falling below a predefined threshold or automatically after a set period
 switch(MESH_DRIVER_WAIVE_CONDITION) 
  {
   case WAIVE_RSSI:       //If the root waive should be triggered by the root's RSSI with the router falling below a predefined threshold                                         

    //Main RSSI waive cycle (that runs while the node is root)
    while(mesh_state.my_type == MESH_ROOT)                                      
     {
      ESP_ERROR_CHECK(esp_wifi_sta_get_ap_info(&routerAP));                                                                 //Retrieve information on the router's AP the root is connected to (to retrieve its current RSSI)
      if((routerAP.rssi <= MESH_DRIVER_WAIVE_RSSI_THRESHOLD)&&(((eventbits = xEventGroupGetBits(mesh_event_group))>>4)&1))  //If the root's RSSI with the router has fallen below the predefined threshold  
       {                                                                                                                    //and a new root election is not already in progress in the mesh network
        ESP_LOGI(TAG,"================================= [Root Waive Driver] =================================");             
        if(!(((eventbits = xEventGroupGetBits(mesh_event_group))>>2)&1))                                                                               //If the root node has no children connected and so is the sole 
         ESP_LOGI(TAG,"The root node would waive for its low RSSI with the router (%i), but it's the sole device in the mesh network",routerAP.rssi);  //device in the mesh network, it has no sense for it to waive
        else
         {
          ESP_LOGI(TAG,"The root node IS WAIVING for its low RSSI with the router (%i), starting a new root election",routerAP.rssi);
          ESP_ERROR_CHECK(esp_mesh_waive_root(&vote_settings,MESH_VOTE_REASON_ROOT_INITIATED));                             //Otherwise perform the root waive, thus triggering a new root election in the mesh network
         }
        ESP_LOGI(TAG,"=======================================================================================\n");
       }
      vTaskDelay(MESH_DRIVER_WAIVE_RSSI_INTERVAL*1000/portTICK_PERIOD_MS);
     }
    break;

   case WAIVE_AUTOMATIC:  //If the root waive should be triggered automatically after a set period
                                                                                  
    vTaskDelay(MESH_DRIVER_WAIVE_AUTOMATIC_INTERVAL*1000/portTICK_PERIOD_MS);                                               //Initial delay (to allow the cycle conditions to be checked right after the  
                                                                                                                            //time period without triggering the root waive immediately the first time)
    //Main automatic waive cycle (that runs while the node is root)
    while(mesh_state.my_type == MESH_ROOT)                                      
     {
      if(((eventbits = xEventGroupGetBits(mesh_event_group))>>4)&1)                                                         //If a new root election is not already in progress in the mesh network
       {
        ESP_LOGI(TAG,"================================= [Root Waive Driver] =================================");
        if(!(((eventbits = xEventGroupGetBits(mesh_event_group))>>2)&1))                                                    //If the root node has no children connected and so is the sole
         ESP_LOGI(TAG,"The root node would waive automatically, but it's the sole device in the mesh network");             //device in the mesh network, it has no sense for it to waive
        else
         {
          ESP_LOGI(TAG,"The root node IS WAIVING automatically");
          ESP_ERROR_CHECK(esp_mesh_waive_root(&vote_settings,MESH_VOTE_REASON_ROOT_INITIATED));                             //Otherwise perform the root waive, thus triggering a new root election in the mesh network
         }
        ESP_LOGI(TAG,"=======================================================================================\n");
       }
      vTaskDelay(MESH_DRIVER_WAIVE_AUTOMATIC_INTERVAL*1000/portTICK_PERIOD_MS);
     }
    break;
  }
 ESP_LOGE(TAG,"The node is no longer root, Waive Driver shutting down");
 xEventGroupClearBits(mesh_event_group,MESH_DRIVER_ROOT_WAIVE);
 vTaskDelete(NULL);
}

/*************************************************************************
 *                Driver Components Starting Functions
 *************************************************************************/

/* Starts the internal driver components, i.e. the driver components that don't require a node to access the external DS
   (called at the end of the MESH_EVENT_PARENT_CONNECTED event specific handler)                                         */
void startMeshInternalDrivers() 
{
 const static char* TAG = "[startMeshInternalDrivers]";
 EventBits_t eventbits;                                   //Used to check the flags in the mesh event group
 static TaskHandle_t logging_handler;                     //Logging Driver task handler
 static TaskHandle_t sender_handler;                      //Sender Driver task handler
 static TaskHandle_t receiver_handler;                    //Receiver Driver task handler

 /* For each internal driver component, if the related task is already running kill it before creating it again */
 if(MESH_DRIVER_LOGGING_ENABLE)                                                          //If the Logging Driver should be executed on the node                        
  {
   if(((eventbits = xEventGroupGetBits(mesh_event_group))>>5)&1)                         //If the Logging Driver task is already running (MESH_DRIVER_INTERNAL_LOGGING)
    {
     ESP_LOGW(TAG," -- Deleting the existing Logging Driver task");
     vTaskDelete(logging_handler);                                                       //Kill the existing Logging Driver task
    }
   ESP_LOGW(TAG," -- Starting the Logging Driver task");
   xEventGroupSetBits(mesh_event_group,MESH_DRIVER_INTERNAL_LOGGING);                    //Set the MESH_DRIVER_INTERNAL_LOGGING flag in the mesh event group
   xTaskCreate(meshLoggingDriver,"meshloggingdriver",8192,NULL,10,&logging_handler);     //Create (and run) the Logging Driver task
  }

 if(MESH_DRIVER_SENDER_ENABLE)                                                           //If the Sender Driver should be executed on the node
  {
   if(((eventbits = xEventGroupGetBits(mesh_event_group))>>6)&1)                         //If the Sender Driver task is already running (MESH_DRIVER_INTERNAL_SENDER)
    {
     ESP_LOGW(TAG," -- Deleting the existing Sender Driver task");
     vTaskDelete(sender_handler);                                                        //Kill the existing Sender Driver task
    }
   ESP_LOGW(TAG," -- Starting the Sender Driver task");
   xEventGroupSetBits(mesh_event_group,MESH_DRIVER_INTERNAL_SENDER);                     //Set the MESH_DRIVER_INTERNAL_SENDER flag in the mesh event group
   xTaskCreate(meshSenderDriver,"meshsenderdriver",8192,NULL,10,&sender_handler);        //Create (and run) the Sender Driver task
  }

 if(MESH_DRIVER_RECEIVER_ENABLE)                                                         //If the Receiver Driver should be executed on the node
  {
   if(((eventbits = xEventGroupGetBits(mesh_event_group))>>7)&1)                         //If the Receiver Driver task is already running (MESH_DRIVER_INTERNAL_RECEIVER)
    {
     ESP_LOGW(TAG," -- Deleting the existing Receiver Driver task");
     vTaskDelete(receiver_handler);                                                      //Kill the existing Receiver Driver task
    }
   ESP_LOGW(TAG," -- Starting the Receiver Driver task");
   xEventGroupSetBits(mesh_event_group,MESH_DRIVER_INTERNAL_RECEIVER);                   //Set the MESH_DRIVER_INTERNAL_RECEIVER flag in the mesh event group
   xTaskCreate(meshReceiverDriver,"meshreceiverdriver",8192,NULL,10,&receiver_handler);  //Create (and run) the Receiver Driver task
  }
 return;
}

/* Starts the external driver components, i.e. the driver components that require a node to access the external DS
   (called at the end of the MESH_EVENT_TODS_STATE event specific handler if a node is informed that the external DS is reachable) */
void startMeshExternalDrivers()
{
 const static char* TAG = "[startMeshExternalDrivers]";
 EventBits_t eventbits;                                   //Used to check the flags in the mesh event group
 static TaskHandle_t senderDS_handler;                    //SenderDS Driver task handler

 /* For each external driver component, if the related task is already running kill it before creating it again */
 if(MESH_DRIVER_SENDERDS_ENABLE)                                                         //If the SenderDS Driver should be executed on the node                                    
  {
   if(((eventbits = xEventGroupGetBits(mesh_event_group))>>8)&1)                         //If the SenderDS Driver task is already running (MESH_DRIVER_EXTERNAL_SENDERDS)
    {
     ESP_LOGW(TAG," -- Deleting the existing SenderDS Driver task");
     vTaskDelete(senderDS_handler);                                                      //Kill the existing SenderDS Driver task
    }
   ESP_LOGW(TAG," -- Starting the SenderDS Driver task");
   xEventGroupSetBits(mesh_event_group,MESH_DRIVER_EXTERNAL_SENDERDS);                   //Set the MESH_DRIVER_EXTERNAL_SENDERDS flag in the mesh event group
   xTaskCreate(meshSenderDSDriver,"meshsenderdsdriver",8192,NULL,10,&senderDS_handler);  //Create (and run) the SenderDS Driver task
  }
 return;
}

/* Starts the root driver components, i.e. the driver components that must be executed on the root node of the mesh network only 
   (called at the end of the MESH_EVENT_ROOT_GOT_IP event specific handler)                                                      */
void startMeshRootDrivers()    
{
 const static char* TAG = "[startMeshRootDrivers]";
 EventBits_t eventbits;                                   //Used to check the flags in the mesh event group
 static TaskHandle_t receiverDS_handler;                  //ReceiverDS Driver task handler
 static TaskHandle_t waive_handler;                       //Waive Driver task handler

 /* For each root driver component, if the related task is already running kill it before creating it again */
 if(MESH_DRIVER_RECEIVERDS_ENABLE)                                                       //If the ReceiverDS Driver should be executed on the node                
  {
   if(((eventbits = xEventGroupGetBits(mesh_event_group))>>9)&1)                         //If the ReceiverDS Driver task is already running (MESH_DRIVER_ROOT_RECEIVERDS)
    {
     ESP_LOGW(TAG," -- Deleting the existing Root ReceiverDS Driver task");
     vTaskDelete(receiverDS_handler);                                                    //Kill the existing ReceiverDS Driver task
    }
   ESP_LOGW(TAG," -- Starting the Root ReceiverDS Driver task");
   xEventGroupSetBits(mesh_event_group,MESH_DRIVER_ROOT_RECEIVERDS);                     //Set the MESH_DRIVER_ROOT_RECEIVERDS flag in the mesh event group
   xTaskCreate(meshReceiverDSDriver,"meshreceiverdsdriver",8192,NULL,10,&receiverDS_handler);  //Create (and run) the ReceiverDS Driver task
  }

 if(MESH_DRIVER_WAIVE_ENABLE)                                                            //If the Waive Driver should be executed on the node
  {
   if(mesh_state.org != SELF_ORGANIZED)                                                  //Note that a root node may waive its root status in Self-Organized mesh networks only
    ESP_LOGW(TAG," -- Root Waive Driver NOT available in a non-Self-Organized mesh network");
   else
    {
     if(((eventbits = xEventGroupGetBits(mesh_event_group))>>10)&1)                      //If the Waive Driver task is already running (MESH_DRIVER_ROOT_WAIVE)
      {
       ESP_LOGW(TAG," -- Deleting the existing Root Waive Driver task");
       vTaskDelete(waive_handler);                                                       //Kill the existing Waive Driver task
      }
     ESP_LOGW(TAG," -- Starting the Root Waive Driver task");
     xEventGroupSetBits(mesh_event_group,MESH_DRIVER_ROOT_WAIVE);                        //Set the MESH_DRIVER_ROOT_WAIVE flag in the mesh event group
     xTaskCreate(meshWaiveDriver,"meshwaivedriver",4096,NULL,10,&waive_handler);         //Create (and run) the Waive Driver task
    } 
  }
 return;
}