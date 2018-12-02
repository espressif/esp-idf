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
#include "meshbasics.h"                         //Main project library
#include "meshbasics_setup_utils.c"             //Setup Module utility functions
#include "meshbasics_setup_menuconfig.h"        //Setup Module menuconfig parameters

/*************************************************************************
 *                           Global Variables                  
 *************************************************************************/
EventGroupHandle_t mesh_event_group;            //Mesh Event Group handler
mesh_status_t mesh_state = {0};                 //Describes the node's current status in the mesh network

static unsigned int failed_attempts;            //Number of failed reconnection attempts to the node's previous parent (used in Manual Networking)

/*************************************************************************
 *                    Mesh Events Specific Handlers
 *************************************************************************/
 
/**====================== Mesh Main Setup Events ======================**/

/* Raised after mesh networking is enabled on a node by calling the esp_mesh_start() function */
void mesh_STARTED_handler()                                                 
{
 const static char* TAG = "[MESH_STARTED handler]";
 wifi_config_t parent_ap = {0};                                             //Used to manually connect the fixed root of a Fixed-Root network to the external router's AP

 //Manual Networking-related variables
 wifi_scan_config_t scan_config = {0};                                      //Used to store the configuration of the Wi-Fi scan to perform to search for a suitable parent for the node
 char router_ssid[33];                                                      //Used to store the external router's SSID should it represent the only suitable parent for the node
 uint8_t designated_mac[6];                                                 //Used to store the specific parent node's or the external router's BSSID should one of them represent the only suitable parent for the node
 char manual_str[300];                                                      //Used to log the type of parent(s) that will be searched for the node in the Wi-Fi scan

 esp_log_level_set("mesh",ESP_LOG_INFO);                                    //Re-enable the mesh stack default logging (temporarily)
 xEventGroupSetBits(mesh_event_group,MESH_ON);                              //Set the MESH_ON flag in the mesh event group
 mesh_reset_status();                                                       //Reset the node's mesh status and the flags in the mesh event group to a set of default parameters representing a node that is not connected to a parent
 switch(mesh_state.org)                                                     //Further actions depend on the node's mesh organization mode
  {
   /* For Self-Organized networking the implicit handler of this event has already started the automatic procedure of searching for a preferred parent to connect to, an idle 
      node broacasting in its Mesh IE the same MID to start an election with, or the router to connect to should none of the previous options be found in a number or scans 
      equal to the rounds in a root node election (plus a fixed number of extra scans), and so no further action is required in the application-level handling of this event */
   case SELF_ORGANIZED:                                                     
    ESP_LOGW(TAG,"Mesh networking enabled in SELF-ORGANIZED mode");
    ESP_LOGW(TAG,"Searching for a preferred parent, a node broadcasting the same MID or the external router");
    break;

   /* For Fixed-Root networking, if a node recognizes itself as the fixed root of the network it must attempt to connect directly to the external router, otherwise is a node is not the fixed-root
      no further action is required in the application-level handling of this event since its implicit handler has already started searching for a preferred parent for the node to connect to      */
   case FIXED_ROOT:                                                          
    ESP_LOGW(TAG,"Mesh networking enabled in FIXED-ROOT mode");               
    if(MESH_IS_NODE_FIXED_ROOT)                                             //Note that in this example the criteria used by a node to check whether it represents the fixed-root of the network or not
     {                                                                      //consists in simply checking a constant set via the menuconfig utility, even if in general a more complex algorithm could be used
      ESP_LOGW(TAG,"The node IS the fixed root, trying to connect to the router");
      strcpy((char*)parent_ap.sta.ssid,MESH_ROUTER_SSID);                   //Copy the fixed root's router settings in the parent_ap struct to attempt to manually connect to the external router
      strcpy((char*)parent_ap.sta.password,MESH_ROUTER_PASSWORD);
	  if(MESH_ROUTER_SPECIFIC_BSSID)                                        
	   {
	    parent_ap.sta.bssid_set = true;
        strToMAC(MESH_ROUTER_BSSID,parent_ap.sta.bssid);
	   }
      parent_ap.sta.channel = MESH_WIFI_CHANNEL;
      ESP_ERROR_CHECK(esp_mesh_set_parent(&parent_ap,NULL,MESH_ROOT,1));    //Attempt to connect to the external router to form a new mesh network (Note that if the external router is not in range or its AP settings  
     }                                                                      //don't match the ones in the fixed root's router configuration, the node will remain stuck in a loop attempting and failing to connect to it)
    else                                                                    //Otherwise if the node doesn't recognize itself to be the fixed-root of the mesh network no further action is required
     ESP_LOGW(TAG,"The node is NOT the fixed root, searching for a preferred parent to connect to");
    break;                      

   /* For Manual Networking, a Wi-Fi scan must be performed to search for the available parents for the node, where the one that will be attempted to connect to is chosen through an user's custom criteria */
   case MANUAL_NETWORKING:
    ESP_LOGW(TAG,"Mesh networking enabled in MANUAL NETWORKING mode");

    //Set the configuration of the Wi-Fi scan to perform according to the parent the node should attempt to connect to
    scan_config.show_hidden = 1;                           //To allow mesh nodes to be found in the scan, APs with hidden SSID must be included in the scan results
    scan_config.channel = MESH_WIFI_CHANNEL;               //Scan on the Mesh Wi-Fi Channel only
    scan_config.scan_type = WIFI_SCAN_TYPE_PASSIVE;        //Passive Wi-Fi scans should be used in manual mesh networking  /**TODO Probably simple due to having a greater dwell time per channel than active scans by default */

    sprintf(manual_str,"Performing a scan on the Mesh Wi-Fi Channel (%i)",MESH_WIFI_CHANNEL);
    if(MESH_MANUAL_PARENT_SEARCH_PREFERRED)   //If a preferred parent for the node should be searched for in any case
     {
      sprintf(manual_str + strlen(manual_str)," searching for the available parents");
      ESP_LOGW(TAG,"%s",manual_str);
      if(MESH_MANUAL_PARENT_SPECIFIC)         //If connecting to a specific parent node is preferred
       ESP_LOGW(TAG,"(where a connection to the specific parent node with BSSID = %s is preferred)",MESH_MANUAL_PARENT_SPECIFIC_BSSID);
      if(MESH_MANUAL_PARENT_ROUTER)           //If connecting to the external router is preferred
       ESP_LOGW(TAG,"(where a connection to the external router as root is preferred)");
     }
    else
     {
      if(MESH_MANUAL_PARENT_SPECIFIC)         //If the node should attempt to connect to a specific parent node only
       {
        sprintf(manual_str + strlen(manual_str)," searching for the specific parent node with BSSID = %s",MESH_MANUAL_PARENT_SPECIFIC_BSSID);
        ESP_LOGW(TAG,"%s",manual_str);
        strToMAC(MESH_MANUAL_PARENT_SPECIFIC_BSSID,designated_mac);       
        scan_config.bssid = designated_mac;   //Set the Wi-Fi scan to search for an AP with a specific BSSID only
       }
      else
       if(MESH_MANUAL_PARENT_ROUTER)          //If the node should attempt to connect to the external router only
        {
         sprintf(manual_str + strlen(manual_str)," searching for the external router with SSID = %s",MESH_ROUTER_SSID);
         strcpy(router_ssid,MESH_ROUTER_SSID);
         scan_config.ssid = (uint8_t*)router_ssid; //Set the Wi-Fi scan to search for an AP with a specific SSID only
         if(MESH_ROUTER_SPECIFIC_BSSID)
          {
           sprintf(manual_str + strlen(manual_str),", BSSID = %s",MESH_ROUTER_BSSID);
           strToMAC(MESH_ROUTER_BSSID,designated_mac);
           scan_config.bssid = designated_mac;     //Set the Wi-Fi scan to search for an AP with a specific BSSID only
           }
         sprintf(manual_str + strlen(manual_str)," to connect to as root");
         ESP_LOGW(TAG,"%s",manual_str);
        }
     }
    ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config,false));  //Start the Wi-Fi scan
    break;

   default:
    ESP_LOGE(TAG,"Mesh networking enabled in unknown organization mode \"%u\", aborting.",mesh_state.org);
    abort();
    break;
  }
 printf("\n");
 return;
}

/* Raised after mesh networking became disabled on a node, which may occur either intentionally by calling
   the esp_mesh_stop() function (not used in this example) or due to a fatal error in the mesh stack       */
void mesh_STOPPED_handler()
{
 const static char* TAG = "[MESH_STOPPED handler]";

 mesh_reset_status();                                                       //Reset the node's mesh status and the flags in the mesh event group to a set of default parameters representing a node that is not connected to a parent
 xEventGroupClearBits(mesh_event_group,MESH_ON);                            //Clear the MESH_ON flag in the mesh event group
 ESP_LOGW(TAG,"Mesh networking became disabled, trying to reenable\n");     //Since in this example the disabling of mesh networking is always unintentional, as an   
 ESP_ERROR_CHECK(esp_mesh_start());                                         //error recovery attempt we to try to reenable it by calling the esp_mesh_start() function
 return;
}

/* Raised after the mesh stack completes a Wi-Fi scan requested by the application (Manual Networking only)
   (in this example a dedicated task is used to handle this event to prevent from running out of stack memory) */
void mesh_SCAN_DONE_handler(void* ap_found)
{
 static const char* TAG = "[SCAN_DONE handler]";

 wifi_scan_config_t scan_config = {0};    //Used to store the configuration of the new Wi-Fi scan to perform should no suitable parent were found for the node
 mesh_cfg_t mesh_config;                  //Used to store the node's base mesh configuration
 uint8_t specific_mac[6];                 //Used to store the BSSID of the node's specific parent
 char ap_auth_str[14];                    //Used to print the authmode of the APs that were found in the scan as strings
 char node_type_str[25];                  //Used to print the type of the mesh nodes that were found in the scan as strings
 char buf[300];                           //Used to print the information of each of the APs that were found in the scan
 
 //Parent connection attempt variables 
 bool parent_found = false;               //If a viable parent to attempt to connect to was found for the node
 wifi_config_t parent = {0};              //Information on the parent's AP to connect to (the node's station base configuration)
 int my_layer = -1;                       //The supposed mesh layer of the node should it successfully connect to its parent
 mesh_type_t my_type = MESH_IDLE;         //The supposed type of the node should it successfully connect to its parent

 //Information on the APs that were found in the scan
 uint16_t ap_num = *(uint32_t*)ap_found;  //Number of APs that were found in the Wi-Fi scan
 wifi_ap_record_t ap_rec;                 //Used to store the record of each of the APs that were found in the scan
 mesh_assoc_t ap_ie;                      //Used to store the Mesh IE of each of the nodes that were found in the scan
 int ap_ie_len = 0;                       //The length in bytes of the vendor IE of each of the APs that were found in the scan (used to determine whether an AP that was found represents a mesh node or not)
 int ap_conn = -1;                        //Used to check if a connection attempt is possible for each of the APs that were found in the scan

 //Information on the specific parent node to possibly attempt to connect to
 wifi_ap_record_t specific_rec;           //AP record of the specific parent node (if found)
 mesh_assoc_t specific_ie;                //Mesh IE of the specific parent node (if found)
 int specific_ind = -1;                   //Whether the specific parent node was found and its index in the AP list
 int specific_conn = -1;                  //Whether the specific parent node was found and if it's possible to attempt to connect to it
 char specific_conn_err[100];             //Used to print the reason an attempt to connect to the node's specific parent cannot be performed

 //Information on the external router to possibly attempt to connect to
 wifi_ap_record_t router_rec;             //AP record of the external router (if found)
 int router_ind = -1;                     //Whether the external router was found and its index in the AP list
 int router_conn = -1;                    //Whether the external router was found and if it's possible to attempt to connect to it
 int root_ind = -1;                       //Whether a root node already present in the mesh network was found and its index in the AP list
 char router_conn_err[100];               //Used to print the reason an attempt to connect to the external router cannot be performed

 //Information on the node's preferred parent to possibly attempt to connect to
 wifi_ap_record_t preferred_rec;          //AP record of the node's preferred parent (if found)
 mesh_assoc_t preferred_ie;               //Mesh IE of the node's preferred parent (if found)         
 int preferred_ind = -1;                  //Whether a preferred parent for the node was found and its index in the AP list               
 int preferred_conn = -1;                 //Whether a preferred parent for the node was found and if it's possible to attempt to connect to it                 

 esp_log_level_set("mesh",ESP_LOG_NONE);  //Disable the mesh stack default logging
 failed_attempts = 0;                     //Reset the number of failed reconnection attempts to the node's previous parent

 if(ap_num>0)    //If at least one AP was found in the scan
  {  
   //Initialize the BSSID of the specific parent node or the external router to attempt to connect to, if any             
   if(MESH_MANUAL_PARENT_SPECIFIC)
    strToMAC(MESH_MANUAL_PARENT_SPECIFIC_BSSID,specific_mac);
   ESP_ERROR_CHECK(esp_mesh_get_config(&mesh_config));  //Retrieve the node's base mesh configuration
 
   ESP_LOGW(TAG,"The following %u APs were found in the scan on the Mesh Wi-Fi Channel:",ap_num);
   ESP_LOGW(TAG,"==========================================================================================================================");
   for(int i=0;i<ap_num;i++)                            //For each AP that was found in the scan
    {
     esp_mesh_scan_get_ap_ie_len(&ap_ie_len);          //Retrieve the AP's vendor IE length
     ap_conn = -1;                                     //Initialize the variable that represents whether it is possible to attempt to connect to the AP in question
     buf[0] = '\0';   
     
     //If it's the AP of a mesh node
     if(ap_ie_len == sizeof(mesh_assoc_t)) 
      {
       esp_mesh_scan_get_ap_record(&ap_rec,&ap_ie);  //Retrieve the node AP's record and Mesh IE
	   
       //Perform a set of checks to determine whether it's possible to attempt to connect to the node
       if(!(checkArrayEqual(ap_ie.mesh_id,mesh_config.mesh_id.addr,6))) //If the node belongs to another mesh network (different MID) 
        {                                             
         ap_conn = 1;      
         strcpy(buf,"(");
        }
       else
        if(ap_ie.mesh_type == MESH_IDLE)  //If the node is an idle node
         ap_conn = 2;
        else
         if((!ap_ie.layer_cap)||(ap_ie.layer >= esp_mesh_get_max_layer())) //If the node is a leaf node
          ap_conn = 3;
         else
          if((ap_ie.assoc>=ap_ie.assoc_cap)||(ap_ie.assoc>=mesh_config.mesh_ap.max_connection)) //If the node's maximum children connections have been reached
           ap_conn = 4;
          else
           if(ap_rec.rssi<MESH_MANUAL_PARENT_RSSI_THRESHOLD) //If the RSSI with the node is below the required threshold
            ap_conn = 5;
           else           //Otherwise it is possible to attempt to connect to the node
            ap_conn = 0; 

       //Prepare a string summarizing the information on the node that was found
       typeToStr(ap_ie.mesh_type,node_type_str);                              //Convert the found node's type to string
       sprintf(buf + strlen(buf),"%s NODE:",node_type_str);         
       sprintf(buf + strlen(buf)," BSSID = "MACSTR,MAC2STR(ap_rec.bssid));
       sprintf(buf + strlen(buf),", MID = "MACSTR,MAC2STR(ap_ie.mesh_id));
       sprintf(buf + strlen(buf),", Layer = %u",ap_ie.layer);
       sprintf(buf + strlen(buf)," (max = %u)",ap_ie.layer+ap_ie.layer_cap);
       sprintf(buf + strlen(buf),", Children = %u",ap_ie.assoc);
       sprintf(buf + strlen(buf),", (max = %u)",ap_ie.assoc_cap);
       sprintf(buf + strlen(buf),", RSSI = %i",ap_rec.rssi);
       switch(ap_conn)         //Append a letter to denote the reason a connection to such node cannot be attempted, if any
        {
         case 1:
          strcat(buf,") (M)"); //The node belongs to another mesh network (different MID) 
          break;
         case 2: 
          strcat(buf," (I)");  //The node is an idle node
          break;
         case 3: 
          strcat(buf," (L)");  //The node is a leaf node
          break;
         case 4: 
          strcat(buf," (C)");  //The node's maximum children connections have been reached
          break;
         case 5: 
          strcat(buf," (R)");  //The RSSI with the node is below the required threshold
          break;
         default:
          break;
        }

       //Check if the node represents the specific parent to attempt to connect to, if any
       if((MESH_MANUAL_PARENT_SPECIFIC)&&(checkArrayEqual(ap_rec.bssid,specific_mac,6))) 
        {
         if((specific_conn<0)||((!ap_conn)&&(ap_rec.rssi>specific_rec.rssi))) //This check is to account for nodes broadcasting the same BSSID (which shoudn't be)
          {
           memcpy(&specific_rec,&ap_rec,sizeof(wifi_ap_record_t));  //Update the record of the specific parent node to attempt to connect to
           memcpy(&specific_ie,&ap_ie,sizeof(mesh_assoc_t));        //Update the Mesh IE of the specific parent node to attempt to connect to
           specific_conn = ap_conn;                                 //Update whether it's possible to attempt to connect to the specific parent node and the reason if it's not 
           specific_ind = i;                                        //Update the index of the specific parent node in the AP list
          }
         strcat(buf," (P)");   //Denotes that this entry is relative the specific parent the node should attempt to connect to
        }
       else                    //If the node it's not the specific parent but an attempt to connect to it can be performed 
        if(!ap_conn)
         strcat(buf," (N)");   //Denotes that this entry is relative to a node which can be attempted to connect to

       //Check whether the node represents the (or in general a better) preferred parent for this node
       //(Remember that a node's preferred parent is given by the node on the shallowest mesh layer with the least children connected and with the highest RSSI, in this order)
       if((!ap_conn) &&
         ((preferred_conn<0) || (ap_ie.layer<preferred_ie.layer) || ((ap_ie.layer==preferred_ie.layer)&&(ap_ie.assoc<preferred_ie.assoc)) || 
         ((ap_ie.layer==preferred_ie.layer)&&(ap_ie.assoc<preferred_ie.assoc)&&(ap_rec.rssi>preferred_rec.rssi))))
        {
         memcpy(&preferred_rec,&ap_rec,sizeof(wifi_ap_record_t));   //Update the record of the preferred parent to possibly attempt to connect to
         memcpy(&preferred_ie,&ap_ie,sizeof(mesh_assoc_t));         //Update the Mesh IE of the preferred parent to possibly attempt to connect to
         preferred_conn = ap_conn;                                  //Update whether it's possible to attempt to connect to such preferred parent
         preferred_ind = i;                                         //Update the index of the node's preferred parent in the AP list
        }
       
       //Check whether the node is the root of the same mesh network (used when a connection to the external router is preferred but a fallback on a preferred parent is intended if a root node is found)
       if((ap_conn!=1)&&(ap_ie.mesh_type == MESH_ROOT))
        root_ind = i;                                               //Update the index of root node in the AP list
      }

     //If it's NOT the AP of a mesh node (and so can be relative to the external router or another network in general)
     else
      { 
       esp_mesh_scan_get_ap_record(&ap_rec,NULL);  //Retrieve the external AP's record only 
    
       //Check whether the AP is relative to the external router and if it's possible to attempt to connect to it
       if(!((!strcmp((char*)ap_rec.ssid,(char*)mesh_config.router.ssid))&&((!MESH_ROUTER_SPECIFIC_BSSID)||(checkArrayEqual(ap_rec.bssid,mesh_config.router.bssid,6))))) //If the AP is not relative to the external 
        {                                                                                                                                                               //router (i.e. it's another network)
         ap_conn = 6;     
         strcpy(buf,"(");
        }
       else            //The AP IS relative to the external router
        if(ap_rec.rssi<MESH_MANUAL_PARENT_RSSI_THRESHOLD)           //If the RSSI with the router is below the required threshold
         ap_conn = 5;
        else           //Otherwise it is possible to attempt to connect to the external router
         ap_conn = 0;    

       //Prepare a string summarizing the information on the AP that was found
       if(strlen((char*)ap_rec.ssid)==0)                            //Whether the SSID was hidden in the AP's beacon frame 
        sprintf(buf + strlen(buf),"SSID = (hidden)");                     
       else
        sprintf(buf + strlen(buf),"SSID = %s",ap_rec.ssid);                     
       sprintf(buf + strlen(buf),", BSSID = "MACSTR,MAC2STR(ap_rec.bssid));
       authToStr(ap_rec.authmode,ap_auth_str);                      //Convert the AP Authmode to string
       sprintf(buf + strlen(buf),", Authmode = %s",ap_auth_str);
       sprintf(buf + strlen(buf),", RSSI = %idb",ap_rec.rssi);
       switch(ap_conn)                                              //Append a letter to denote the reason a connection to such AP cannot be attempted, if any
        {
         case 6:
          strcat(buf,") (O)"); //The AP is not relative to the external router (i.e. it's another network) 
          break;
         case 5: 
          strcat(buf," (R)");  //The RSSI with the router is below the required threshold
          break;
         default:
          break;
        }
       
       //Check whether the information relative to the external router should be updated
       if((ap_conn == 5)||(ap_conn==0))
        {
         if((router_conn<0)||((!ap_conn)&&(ap_rec.rssi>router_rec.rssi)))  //This check is to account for external routers broadcasting the same SSID
          {
           memcpy(&router_rec,&ap_rec,sizeof(wifi_ap_record_t));           //Update the record of the external router
           router_conn = ap_conn;                                          //Update whether it's possible to attempt to connect to the external router and the reason if it's not
           router_ind = i;                                                 //Update the index of the external router in the AP list
          }
         strcat(buf," (E)");                                               //Denotes that this entry is relative the external router
        }
      }
     ESP_LOGW(TAG,"%i) %s",i+1,buf);                                       //Print the summary information on the AP (node or not) that was found
    }
   ESP_LOGW(TAG,"--------------------------------------------------------------------------------------------------------------------------");
   ESP_LOGW(TAG,"(P) = Specific Parent node, (E) = External Router, (N) = Suitable Parent Node, (M) = Node in another Mesh Network");
   ESP_LOGW(TAG,"(I) = Idle Node, (L) = Maximum Layer Reached, (C) = Maximum Children Reached, (R) = Insufficient RSSI, (O) = Other Network");
   ESP_LOGW(TAG,"==========================================================================================================================");

   /*-- Perform actions based on the APs that were found in the scan and the parent the node should attempt to connect to --*/

   //Convert to string the reason a connection to the specific parent node and/or the external router cannot be attempted, if any    
   if(MESH_MANUAL_PARENT_SPECIFIC&&specific_conn)
    connErrToStr(specific_conn,specific_conn_err);
   if(MESH_MANUAL_PARENT_ROUTER&&router_conn)
    connErrToStr(router_conn,router_conn_err);

   //If the node should attempt to connect to a specific parent node
   if(MESH_MANUAL_PARENT_SPECIFIC)
     {
      if(!specific_conn) //If the specific parent node was found and the connection attempt is possible
       {
        ESP_LOGW(TAG,"The specific node with BSSID = %s was found in the scan (%i), attempting to connect to it",MESH_MANUAL_PARENT_SPECIFIC_BSSID,specific_ind+1);
        parent_found = true;                                                        //Set that the connection to a parent must be attempted
        setParentNode(&specific_rec,&specific_ie,&parent.sta,&my_type,&my_layer);   //Set the target of the parent connection attempt to the specific parent node  
       }                                                                            
      else
       if(specific_conn != -1)  //If the specific parent node was found but a connection attempt is not possible, print the reason why 
        ESP_LOGW(TAG,"The specific node with BSSID = %s was found in the scan (%i)%s",MESH_MANUAL_PARENT_SPECIFIC_BSSID,specific_ind+1,specific_conn_err);
       else                     //If the specific parent node was not found
        ESP_LOGW(TAG,"The specific node with BSSID = %s was not found in the scan",MESH_MANUAL_PARENT_SPECIFIC_BSSID);
     }

   //If the node should attempt to connect to the external router
   if(MESH_MANUAL_PARENT_ROUTER)
    {
     if(!router_conn)  //If the external router was found
      {
       if((root_ind == -1)||(!MESH_MANUAL_PARENT_SEARCH_PREFERRED)) //If the root node of the mesh network was not found or the fallback to a preferred parent is not desidered
        {
         ESP_LOGW(TAG,"The external router was found in the scan (%i), attempting to connect to it",router_ind+1);
         parent_found = true;                                           //Set that the connection to a parent must be attempted   
         setParentRouter(&router_rec,&parent.sta,&my_type,&my_layer);   //Set the target of the parent connection attempt to the external router
        }     
       else  //If the root node of the mesh network was found and the fallback on a preferred parent is desidered
        ESP_LOGW(TAG,"The external router was found in the scan (%i), but a root node is already present in the mesh network",router_ind+1);
      }
     else
      if(router_conn != -1)  //If the external was found but a connection attempt is not possible, print the reason why (currently because its RSSI is below the required threshold only)
       ESP_LOGW(TAG,"The external router was found in the scan (%i)%s",router_ind+1,router_conn_err);
      else                   //If the external router was not found
       {
        sprintf(buf,"The external router with SSID = %s",(char*)mesh_config.router.ssid);
        if(MESH_ROUTER_SPECIFIC_BSSID)
         sprintf(buf + strlen(buf),", BSSID = "MACSTR,MAC2STR(mesh_config.router.bssid));
        ESP_LOGW(TAG,"%s was not found in the scan",buf);
       }
    }

   //If a parent to attempt to connect to has not already been found and connecting to the node's prefered parent is allowed
   if(!parent_found&&MESH_MANUAL_PARENT_SEARCH_PREFERRED)
    {
     if(!preferred_conn)                                               //If a preferred parent for the node was found
      {
       if((MESH_MANUAL_PARENT_SPECIFIC)||(MESH_MANUAL_PARENT_ROUTER))  //If connecting to the preferred parent is used as a fallback because the specific parent node or the external router were not found
        ESP_LOGW(TAG,"Attempting to connect to the node with BSSID = "MACSTR" (%i) as a fallback",MAC2STR(preferred_rec.bssid),preferred_ind+1);
       else                                                            //Otherwise if connecting the node to it's preferred parent was intended in the first place
        ESP_LOGW(TAG,"Attempting to connect to preferred parent with BSSID = "MACSTR" (%i)",MAC2STR(preferred_rec.bssid),preferred_ind+1);
       parent_found = true;                                                          //Set that the connection to a parent must be attempted 
       setParentNode(&preferred_rec,&preferred_ie,&parent.sta,&my_type,&my_layer);   //Set the target of the parent connection attempt to the preferred parent
      }
     else                                                              //If a preferred parent for the node was not found
      if(!router_conn)                                                 //If the external router was found
       {
        if(MESH_MANUAL_PARENT_SPECIFIC)                                //If connecting to the external router is used as a fallback because the specific parent node was not found
         ESP_LOGW(TAG,"Attempting to connect to the external router (%i) as root as a fallback",router_ind+1);
        else                                                           //If connecting to the external router is used as a fallback because no preferred parent was found
         ESP_LOGW(TAG,"Attempting to connect to the external router (%i) as root",router_ind+1);
        parent_found = true;                                           //Set that the connection to a parent must be attempted 
        setParentRouter(&router_rec,&parent.sta,&my_type,&my_layer);   //Set the target of the parent connection attempt to the external router
       }
      else                                                             //If neither a preferred parent nor the external router were found
       if((MESH_MANUAL_PARENT_SPECIFIC)||(MESH_MANUAL_PARENT_ROUTER))  //If the connection to the preferred parent or to the external router was used as a fallback because the specific parent node (or router) was not found
        ESP_LOGW(TAG,"No suitable parent to fall back to was also found in the scan");
       else                                                            //Otherwise if connecting the node to it's preferred parent (or the router) was intended in the first place
        ESP_LOGW(TAG,"No suitable parent node or router to connect to was found in the scan");
    }
  }
 else  //If no APs were found in the scan
  ESP_LOGW(TAG,"No APs were found in the scan on the Mesh Wi-Fi Channel");

 //Whether a suitable parent for the node is found and so a connection should be attempted
 if(parent_found)
  { 
   printf("\n");
   ESP_ERROR_CHECK(esp_mesh_set_parent(&parent,&mesh_config.mesh_id,my_type,my_layer)); //Attempt to connect the node to its designated parent
  }
 else //If no suitable parent for the node was found
  {
   //Prepare the configuration of the Wi-Fi scan to perform to search again for the parent the node should attempt to connect to
   scan_config.show_hidden = 1;                         //To allow mesh nodes to be found in the scan, APs with hidden SSID must be included in the scan results
   scan_config.channel = MESH_WIFI_CHANNEL;             //Scan on the Mesh Wi-Fi Channel only
   scan_config.scan_type = WIFI_SCAN_TYPE_PASSIVE;      //Passive Wi-Fi scans should be used in manual mesh networking  /**TODO Probably simple due to having a greater dwell time per channel than active scans by default */
   if(!MESH_MANUAL_PARENT_SEARCH_PREFERRED)             //If the node should attempt to connect to the specific parent node or the external router only
    {
     if(MESH_MANUAL_PARENT_SPECIFIC)                    //If the node should attempt to connect to the specific parent node only
      scan_config.bssid = specific_mac;                 //Set the Wi-Fi scan to search for an AP with a specific BSSID only
     else
      if(MESH_MANUAL_PARENT_ROUTER)                     //If the node should attempt to connect to the external router only
       {
        scan_config.ssid = mesh_config.router.ssid;     //Set the Wi-Fi scan to search for an AP with a specific SSID only
        if(MESH_ROUTER_SPECIFIC_BSSID)                  //If the node should attempt to connect to an external router with a specific BSSID only
         scan_config.bssid = mesh_config.router.bssid;  //Set the Wi-Fi scan to search for an AP with a specific BSSID only
       }
    }
   ESP_LOGW(TAG,"Performing a new scan on the Mesh Wi-Fi Channel in %ims...\n",MESH_MANUAL_SCAN_RETRY_INTERVAL);
   vTaskDelay(MESH_MANUAL_SCAN_RETRY_INTERVAL/portTICK_PERIOD_MS);  //Wait for a defined interval before performing another Wi-Fi scan
   ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config,false));        //Perform another Wi-Fi scan
  }
 vTaskDelete(NULL); //Kill this task
}

/* Raised after a node succesfully connects to a parent in the mesh network */
void mesh_PARENT_CONNECTED_handler(mesh_event_connected_t* info)
{
 const static char* TAG = "[PARENT_CONNECTED handler]";
 tcpip_adapter_ip_info_t ipinfo;                                            //Possibly used to set a custom static IP configuration for the root node's station interface
 tcpip_adapter_dns_info_t dnsaddr;                                          //Possibly used to set the DNS servers addresses of the root node's station interface

 esp_log_level_set("mesh",ESP_LOG_NONE);                                    //Disable the mesh stack default logging
 xEventGroupSetBits(mesh_event_group,MESH_PARENT);                          //Set the MESH_PARENT flag in the mesh event group
 memcpy(mesh_state.parent_addr,info->connected.bssid,6);                    //Update the parent's MAC address
 mesh_state.my_layer = info->self_layer;                                    //Update the node's mesh layer (which also determines its type as follows)
 if(mesh_state.my_layer == 1)                                               //If the node is ROOT, an IP configuration must be applied to its station interface to allow it to communicate with the external router
  {
   mesh_state.my_type = MESH_ROOT;
   ESP_LOGW(TAG,"Connected to the external router with BSSID = "MACSTR,MAC2STR(mesh_state.parent_addr));
   ESP_LOGW(TAG,"The node is the ROOT node (layer = %i)",mesh_state.my_layer);         
   if(MESH_ROOT_IPDYNAMIC)                                                  //If a dynamic IP configuration is used for the root station interface, (re)enable its station DHCP client
    {
     ESP_LOGW(TAG,"Enabling the station DHCP client to retrieve a dynamic IP configuration");
     ESP_ERROR_CHECK(tcpip_adapter_dhcpc_start(TCPIP_ADAPTER_IF_STA));
    }
   else                                                                     //Otherwise if a static IP configuration is used for the root station interface, apply it
    {
     ESP_LOGW(TAG,"Applying the root station static IP configuration");
     //Set the Root station static IP configuration
     inet_pton(AF_INET,MESH_ROOT_STATIC_IP,&ipinfo.ip);
     inet_pton(AF_INET,MESH_ROOT_STATIC_NETMASK,&ipinfo.netmask);
     inet_pton(AF_INET,MESH_ROOT_STATIC_GATEWAY,&ipinfo.gw);
     ESP_ERROR_CHECK(tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_STA,&ipinfo)); //Apply the static IP configuration

     //Set the Root station DNS servers
     inet_pton(AF_INET,MESH_ROOT_STATIC_DNS_PRIMARY,&dnsaddr.ip);
     ESP_ERROR_CHECK(tcpip_adapter_set_dns_info(TCPIP_ADAPTER_IF_STA,TCPIP_ADAPTER_DNS_MAIN,&dnsaddr)); //Set the Primary DNS Server
     inet_pton(AF_INET,MESH_ROOT_STATIC_DNS_SECONDARY,&dnsaddr.ip);
     ESP_ERROR_CHECK(tcpip_adapter_set_dns_info(TCPIP_ADAPTER_IF_STA,TCPIP_ADAPTER_DNS_BACKUP,&dnsaddr)); //Set the Secondary DNS Server
    }
  }
 else                                                                       //Otherwise if the node is not root, just update its type
  {
   ESP_LOGW(TAG,"Connected to parent node with BSSID = "MACSTR,MAC2STR(mesh_state.parent_addr));                                 
   if(mesh_state.my_layer < MESH_MAX_LAYER)                                 //The node is an INTERMEDIATE PARENT node
    {
     mesh_state.my_type = MESH_NODE;
     ESP_LOGW(TAG,"The node is an INTERMEDIATE PARENT node (layer = %i)",mesh_state.my_layer);
    }
   else                                                                     //The node is a LEAF node (my_layer == MESH_MAX_LAYER)
    {
     mesh_state.my_type = MESH_LEAF;
     ESP_LOGW(TAG,"The node is a LEAF node (layer = %i = MESH_MAX_LAYER)",mesh_state.my_layer);
    }
  }
 startMeshInternalDrivers();                                                //Start the internal driver components, i.e. the driver components that don't require the node to access the external DS
 printf("\n");         
 return;
}

/* Raised after a node disconnects from its parent or fails to connect or automatically reconnect to it */
void mesh_PARENT_DISCONNECTED_handler(mesh_event_disconnected_t* info)
{
 const static char* TAG = "[PARENT_DISCONNECTED handler]";
 EventBits_t eventbits;                                                     //Used to check the flags in the mesh event group
 wifi_scan_config_t scan_config = {0};                                      //Used to store the configuration of the Wi-Fi scan to possibly perform to allow the node to fall back on its preferred parent (used in Manual Networking)
 char disc_reason_str[30];                                                  //Used to print the reason for the disconnection as a string

 esp_log_level_set("mesh",ESP_LOG_INFO);                                    //Re-enable the mesh stack default logging (temporarily)
 failed_attempts++;                                                         //Increment the number of failed reconnection attempts to the node's previous parent (used in Manual Networking)
 staDiscToStr(info->reason,disc_reason_str);                                //Converts the reason for the disconnection to string
 if(((eventbits = xEventGroupGetBits(mesh_event_group))>>1)&1)              //If the MESH_PARENT flag is set, the node disconnected from its parent
  {      
   if(mesh_state.my_type == MESH_ROOT)                                      //If the node is root, inform the mesh network that the external DS is
    {                                                                       //no longer reachable and reset its Station interface IP configuration
     ESP_ERROR_CHECK(esp_mesh_post_toDS_state(false));
     mesh_root_reset_stationIP();
     ESP_LOGW(TAG,"Root disconnected from the router with BSSID = "MACSTR", reason = %s",MAC2STR(info->bssid),disc_reason_str);
    }              
   else
    ESP_LOGW(TAG,"Node disconnected from its parent with BSSID = "MACSTR", reason = %s",MAC2STR(info->bssid),disc_reason_str);                                       
   mesh_reset_status();                                                     //Reset the node's mesh status and the flags in the mesh event group to a set of default parameters representing a node that is not connected to a parent
  }                                                                         
 else                                                                       //If the MESH_PARENT flag is not set, the node failed to connect or automatically reconnect to its parent, and no action is required
  ESP_LOGW(TAG,"Node failed to connect to parent with BSSID = "MACSTR", reason = %s",MAC2STR(info->bssid),disc_reason_str);

 //For Manual Networking, if the maximum number of failed reconnection attempts has been reached and fallbacks on the node's preferred parent are allowed, perform a Wi-Fi scan to search for it
 /** TODO BUG: Falling back on the node's preferred parent is currently not possible since the automatic reconnection attempts */
 /**           to the node's previous parent performed by the mesh stack prevent a new Wi-Fi scan from being carried out       */
 if((mesh_state.org == MANUAL_NETWORKING)&&(MESH_MANUAL_PARENT_SEARCH_PREFERRED)&&(failed_attempts>=MESH_MANUAL_MAX_CONNECTIONS_FAILS))
  {                                         
   ESP_LOGW(TAG,"Performing a Wi-Fi scan searching for a preferred parent to fall back to");
   ESP_ERROR_CHECK(esp_wifi_scan_stop());                                   //Stop a Wi-Fi scan already in progress /**TODO BUG: ineffective, see above */

   //Prepare the configuration of the Wi-Fi scan to perform to search for the node's preferred parent
   scan_config.show_hidden = 1;                                             //To allow mesh nodes to be found in the scan, APs with hidden SSID must be included in the scan results
   scan_config.channel = MESH_WIFI_CHANNEL;                                 //Scan on the Mesh Wi-Fi Channel only
   scan_config.scan_type = WIFI_SCAN_TYPE_PASSIVE;                          //Passive Wi-Fi scans should be used in in manual mesh networking
   ESP_ERROR_CHECK(esp_wifi_scan_start(&scan_config,false));                /**TODO Probably simple due to having a greater dwell time per channel than active scans by default */
  }
 printf("\n");                                                              
 return;
}

/* Raised in a self-organized network and for the non-fixed-root nodes in a fixed-root network is no viable parent is found for a node in a fixed number of scans */
void mesh_NO_PARENT_FOUND_handler(mesh_event_no_parent_found_t* info)       
{                                                                           //Note that it would be possible in this handler to implement advanced error-recovery procedures  
 const static char* TAG = "[NO_PARENT_FOUND handler]";                      //such as performing an all-channel Wi-Fi scan searching for a viable parent on a different 
                                                                            //Mesh Wi-Fi Channel and/or changing the node's mesh configuration (not shown here)
 switch(mesh_state.org)                                                     //Depending on the mesh organization mode inform that no viable parent has been found for the node
  {                                                                         
   case SELF_ORGANIZED:
    ESP_LOGW(TAG,"No preferred parent, idle node broadcasting the same MID, or the router have been found in %i scans, continuing perfoming periodical scans until any is found...",info->scan_times);
    break;
   case FIXED_ROOT:                                                         //Non-fixed-root nodes only
     ESP_LOGW(TAG,"No preferred parent has been found in %i scans, continuing performing periodical scans until any is found...",info->scan_times);
    break;
   case MANUAL_NETWORKING:                                                  //This event is not raised by the mesh stack in Manual Networking mode
    break;
  }
 ESP_LOGW(TAG,"Note that the node's main setup phase (and thus the program's execution) cannot proceed until a viable parent is found\n");
 return;
}

/**==================== Mesh Network Update Events ====================**/

/* Raised after a child connects to a node */
void mesh_CHILD_CONNECTED_handler(mesh_event_child_connected_t* info)
{
 const static char* TAG = "[CHILD_CONNECTED handler]";
 EventBits_t eventbits;                                                     //Used to check the flags in the mesh event group

 if(!(((eventbits = xEventGroupGetBits(mesh_event_group))>>2)&1))           //If in the mesh event group the MESH_CHILD flag is not set, and thus  
  xEventGroupSetBits(mesh_event_group,MESH_CHILD);                          //this is the first child to connect to the node, set the MESH_CHILD flag
 ESP_LOGW(TAG,"New child node with MAC = "MACSTR" connected, aid = %u\n",MAC2STR(info->mac),info->aid);
 return;
}

/* Raised after a child disconnects from a node */
void mesh_CHILD_DISCONNECTED_handler(mesh_event_child_disconnected_t* info) //Raises when a child disconnects from the node
{
 const static char* TAG = "[CHILD_DISCONNECTED handler]";                 
 wifi_sta_list_t children;                                                  //Used to store information on the clients connected to the device's SoftAP interface
 
 ESP_ERROR_CHECK(esp_wifi_ap_get_sta_list(&children));                      //Retrieve information on the clients connected to the device's SoftAP interface
 if(children.num == 0)                                                      //If it was the last child to disconnect from the node, clear the MESH_CHILD flag in the mesh event group 
  xEventGroupClearBits(mesh_event_group,MESH_CHILD);
 ESP_LOGW(TAG,"Child node with MAC = "MACSTR" disconnected, aid = %u\n",MAC2STR(info->mac),info->aid);
 return;
}

/* Raised after a node's descendant (with its possible further descendants) connect to the mesh network */
void mesh_ROUTING_TABLE_ADD_handler(mesh_event_routing_table_change_t* info)
{
 const static char* TAG = "[ROUTING_TABLE_ADD handler]";   

 ESP_LOGW(TAG,"Newly connected descendant added %u entries to the node's routing table (current size = %u)\n",info->rt_size_change,info->rt_size_new);
 return;
}

/* Raised after a node's descendant (with its possible further descendants) disconnect from the mesh network */
/** TODO BUG: Currently this event is also triggered the first time a node connects to a parent, causing the removal of a "bogus" entry from the node's routing table */
void mesh_ROUTING_TABLE_REMOVE_handler(mesh_event_routing_table_change_t* info)
{
 const static char* TAG = "[ROUTING_TABLE_REMOVE handler]";   

 ESP_LOGW(TAG,"Disconnected descendant caused the removal of %u entries from the node's routing table (current size = %u)\n",info->rt_size_change,info->rt_size_new);
 return;
}

/* Raised after a node connects to its parent, which passes it the root's SoftAP MAC address, and whenever a root switch occurs 
   in the network, where the new root's SoftAP MAC address is propagated from parent to children through the entire network     */
void mesh_ROOT_ADDRESS_handler(mesh_event_root_address_t* info)
{
 const static char* TAG = "[ROOT_ADDRESS handler]";

 memcpy(mesh_state.root_addr,info->addr,6);
 if(mesh_state.my_type == MESH_ROOT)
  ESP_LOGW(TAG,"Root node address set to self ("MACSTR")",MAC2STR(mesh_state.root_addr));
 else
  ESP_LOGW(TAG,"Root node address retrieved: "MACSTR,MAC2STR(mesh_state.root_addr));
 printf("\n");
 return;
}

/* Raised after a node connects to a parent node should their fixed root settings differ, where the child will change its own to match the one of its parent */
void mesh_ROOT_FIXED_handler(mesh_event_root_fixed_t* info)
{
 const static char* TAG = "[ROOT_FIXED handler]";

 ESP_LOGW(TAG,"Discrepancy with the parent's fixed root setting");          
 if(info->is_fixed)                                                         //Change the node's mesh organization mode accordingly  
  {                                                                         //(Note that if fixed-root networking is set in this way, the node can only be a non-fixed-root node)
   mesh_state.org = FIXED_ROOT;
   ESP_LOGW(TAG,"Mesh organization mode changed to: FIXED_ROOT");
  }
 else
  {
   mesh_state.org = SELF_ORGANIZED;
   ESP_LOGW(TAG,"Mesh organization mode changed to: SELF_ORGANIZED");
  }
 printf("\n");
 return;
}

/* Raised after a node connects to its parent if the latter was previously informed that the external DS is reachable, or whenever the root node posts an  
   update on its accessibility via the esp_mesh_post_toDS_state() function, information that is propagated from parent to children through the entire network */
void mesh_TODS_STATE_handler(mesh_event_toDS_state_t* info)     
{
 const static char* TAG = "[TODS_STATE handler]";

 if(*info == MESH_TODS_REACHABLE)                                           //If the external DS is reachable
  {
   xEventGroupSetBits(mesh_event_group,MESH_TODS);                          //Set the MESH_TODS flag in the mesh event group                                
   if(mesh_state.my_type == MESH_ROOT)
    ESP_LOGW(TAG,"The mesh network is informed that the external DS is now REACHABLE");
   else
    ESP_LOGW(TAG,"Node informed that the external DS is REACHABLE");
   startMeshExternalDrivers();                                              //Start the external driver components, i.e. the driver components that require the node to access the external DS
  }
 else                                                                       //If the external DS is NOT reachable
  {
   xEventGroupClearBits(mesh_event_group,MESH_TODS);                        //Clear the MESH_TODS flag in the mesh event group
   if(mesh_state.my_type == MESH_ROOT)
    ESP_LOGW(TAG,"The mesh network is informed that the external DS is NO LONGER reachable");
   else
    ESP_LOGW(TAG,"Node informed that the external DS is NO LONGER reachable");
  }
 printf("\n");  
 return;
}

/* Raised when a new root election is started in a self-organized network, which may be caused by the root node self-healing process or by the root node waiving its root status */
/** TODO The info.rc_addr.addr additional information is currently irrelevant (it's always set to 00:00:00:00:00:00) */
/** TODO BUG: While they partecipate as normal in the new root election, this event doesn't trigger on the nodes which detected and so started the root self-healing process */
void mesh_VOTE_STARTED_handler(mesh_event_vote_started_t* info)
{
 const static char* TAG = "[VOTE_STARTED handler]";                         
 char vote_reason_str[18];                                                  //Used to print the reason for the new root election as a string

 esp_log_level_set("mesh",ESP_LOG_INFO);                                    //Re-enable the mesh stack default logging (temporarily)
 xEventGroupClearBits(mesh_event_group,MESH_VOTE);                          //Clear the MESH_VOTE flag (active low, 0 = election in progress, 1 = no election)
 voteReasonToStr(info->reason,vote_reason_str);                             //Convert the reason for the new root election to string
 ESP_LOGW(TAG,"New root election started, reason = %s, rounds = %i\n",vote_reason_str,info->attempts);
 return;
}

/* Raised whan a new root election in a self-organized network ends */
void mesh_VOTE_STOPPED_handler()
{
 const static char* TAG = "[VOTE_STOPPED handler]";  

 esp_log_level_set("mesh",ESP_LOG_NONE);                                    //Disable the mesh stack default logging
 xEventGroupSetBits(mesh_event_group,MESH_VOTE);                            //Set the MESH_VOTE flag (active low, 0 = election in progress, 1 = no election)
 ESP_LOGW(TAG,"The new root election has ended\n");
 return;
}

/* Raised whenever a network topology change affects the ancestors of a node */
void mesh_LAYER_CHANGE_handler(mesh_event_layer_change_t* info)
{
 const static char* TAG = "[LAYER_CHANGE handler]";

 ESP_LOGW(TAG,"The node's mesh layer has changed: %i -> %u",mesh_state.my_layer,info->new_layer);
 mesh_state.my_layer = info->new_layer;                                         //Update the node's mesh layer
 if((mesh_state.my_layer < MESH_MAX_LAYER)&&(mesh_state.my_type != MESH_NODE))  //Check whether the layer change causes the node's type to change from INTERMEDIATE PARENT to LEAF or viceversa
  {                                                                             //(note that changes to the root's type are not taken into account since when a node becomes root it always switches 
   mesh_state.my_type = MESH_NODE;                                              //parents (to the router), and so its type will be updated in the MESH_EVENT_PARENT_CONNECTED event specific handler
   ESP_LOGW(TAG,"Node's type changed from LEAF to INTERMEDIATE PARENT");
  }
 else
  if(mesh_state.my_layer == MESH_MAX_LAYER)
   {
    mesh_state.my_type = MESH_LEAF;
    ESP_LOGW(TAG,"Node's type changed from INTERMEDIATE PARENT to LEAF");
   }                                                                     
 printf("\n");
 return;
}

/* Raised in cascade on all nodes of the mesh network should the external router change its AP Wi-Fi channel */
/** TODO The actual conditions for this event to be raised are unclear, since currently switching the external router's AP Wi-Fi channel causes the root node to disconnect from it and to remain stuck in a loop attempting to
         reconnect (not even eventually triggering the MESH_EVENT_NO_PARENT_FOUND event), even if switching back the external router's AP Wi-Fi channel on the Mesh Wi-Fi channel allows the root node to reconnect normally   */
void mesh_CHANNEL_SWITCH_handler(mesh_event_channel_switch_t* info)
{                                                                           
 const static char* TAG = "[CHANNEL_SWITCH handler]";              

 mesh_state.channel = info->channel;
 ESP_LOGW(TAG,"Mesh Wi-Fi channel changed to channel: %u\n",info->channel);
 return;
}

/**===================== Mesh Root-specific Events =====================**/

/* Raised on the root node once it obtains an IP configuration for its station interface */
void mesh_ROOT_GOT_IP_handler(mesh_event_root_got_ip_t* info)
{
 const static char* TAG = "[ROOT_GOT_IP handler]";

 ESP_LOGW(TAG,"Root node obtained the following IP Configuration for its station interface:");
 ESP_LOGW(TAG,"IP = "IPSTR", Netmask = "IPSTR", Gateway = "IPSTR,IP2STR(&info->ip_info.ip),IP2STR(&info->ip_info.netmask),IP2STR(&info->ip_info.gw));
 startMeshRootDrivers();                                                    //Start the root driver components, i.e. the driver components that must be executed only on the root node in the mesh network
 ESP_ERROR_CHECK(esp_mesh_post_toDS_state(true));                           //Inform the mesh network that the external DS is now accessible
 return;                                                                    //(supposing that if a static IP configuration is used, it allows the root to communicate with the router)                           
}

/* Raised on the current or a former root node once the lease time of its station dynamic IP configuration expires (which for the 
   current root node also implies that its DHCP client failed to renew or otherwise retrieve a new IP configuration for the interface) */
void mesh_ROOT_LOST_IP_handler()
{
 const static char* TAG = "[ROOT_LOST_IP handler]";

 if(mesh_state.my_type == MESH_ROOT)                                        //If the node is the current root
  {
   ESP_ERROR_CHECK(esp_mesh_post_toDS_state(false));                        //Inform the mesh network that the external DS is no longer reachable
   ESP_LOGW(TAG,"The root node's station dynamic IP configuration has expired");
  }
 else                                                                       //Otherwise if the node is a former root, no action is required
  ESP_LOGW(TAG,"This former root node's station dynamic IP configuration has expired");
 printf("\n");
 return;
}

/* Raised on the root node when it receives a root switch request from a candidate root node following the election caused by the root node waiving its root status (Self-Organized networking only) */
/** TODO The info.reason additional information is currently irrelevant (currently this event can only be raised for the reason above) */
void mesh_ROOT_SWITCH_REQ_handler(mesh_event_root_switch_req_t* info)
{
 const static char* TAG = "[ROOT_SWITCH_REQ handler]"; 

 mesh_root_reset_stationIP();                                               //Reset the root node's station IP configuration
 mesh_state.my_type = MESH_NODE;                                            //Change the node's type from root to intermediate parent (temporarily, since the node will disconnect from the router)
 ESP_LOGW(TAG,"The now former root node received a switch request from candidate root with MAC = "MACSTR,MAC2STR(info->rc_addr.addr));
 ESP_LOGW(TAG,"Disconnecting from the router and searching for a preferred parent to connect to\n");
 return;
}

/* Raised on the candidate root node after it receives the root switch acknowledgment from the former root following the election caused by it waiving its root status (Self-Organized networking only) */
void mesh_ROOT_SWITCH_ACK_handler()
{
 const static char* TAG = "[ROOT_SWITCH_ACK handler]"; 

 ESP_LOGW(TAG,"Former root node acknlowedged the root switch");
 ESP_LOGW(TAG,"Disconnecting from the current parent and attempting to connect to the router as root\n");
 return;
}

/* Raised after an internal communication on all root nodes but the one with the highest RSSI with the router whenever a root conflict is detected in a mesh network */
/** TODO It is unclear whether this event can be raised at all in the current ESP-IDF version */
void mesh_ROOT_ASKED_YIELD_handler(mesh_event_root_conflict_t* info)
{
 const static char* TAG = "[ROOT_ASKED_YIELD handler]";

 mesh_root_reset_stationIP();                                               //Reset the root node's station IP configuration
 mesh_state.my_type = MESH_NODE;                                            //Change the node's type from root to intermediate parent (temporarily, since the node will disconnect from the router)
 ESP_LOGW(TAG,"Root Conflict detected in the mesh network");
 ESP_LOGW(TAG,"This root node has been asked to yield by the root node with with MAC = "MACSTR", network capacity = %u and RSSI with the router = %idb",MAC2STR(info->addr),info->capacity,info->rssi);
 ESP_LOGW(TAG,"Disconnecting from the router and searching for a preferred parent to connect to");
 printf("\n");
 return;
}
                                                                     
/*************************************************************************
 *                    Mesh Events General Handler
 *************************************************************************/
void mesh_events_handler(mesh_event_t event)
{
 static const char* TAG = "<EVENT>";
 static uint8_t ap_found;             //Used to store the number of APs found in a Wi-Fi scan (passed as argument to the task that handles the SYSTEM_EVENT_SCAN_DONE event)

 /* Call the specific handler relative to each mesh event raised by the mesh stack, passing the provided additional information provided where applicable */
 switch(event.id)                                                    
  {
   /*-- Mesh Main Setup Events --*/
   case MESH_EVENT_STARTED:
    ESP_LOGW(TAG,"MESH_EVENT_STARTED");
    mesh_STARTED_handler();
    break;
   case MESH_EVENT_STOPPED:
    ESP_LOGW(TAG,"MESH_EVENT_STOPPED");
    mesh_STOPPED_handler();
    break;
   case MESH_EVENT_SCAN_DONE:                                                             
    ESP_LOGW(TAG,"MESH_EVENT_SCAN_DONE");                                                
    ap_found = event.info.scan_done.number;                                               //Set the number of APs that were found in the scan
    xTaskCreate(mesh_SCAN_DONE_handler,"mesh_SCAN_DONE_handler",8192,&ap_found,10,NULL);  //In this application a dedicated task is created to handle the 
    break;                                                                                //MESH_EVENT_SCAN_DONE event to prevent from running out of stack memory
   case MESH_EVENT_PARENT_CONNECTED:
    ESP_LOGW(TAG,"MESH_EVENT_PARENT_CONNECTED");
    mesh_PARENT_CONNECTED_handler(&event.info.connected);
    break;
   case MESH_EVENT_PARENT_DISCONNECTED:
    ESP_LOGW(TAG,"MESH_EVENT_PARENT_DISCONNECTED");
    mesh_PARENT_DISCONNECTED_handler(&event.info.disconnected);
    break;
   case MESH_EVENT_NO_PARENT_FOUND:
    ESP_LOGW(TAG,"MESH_EVENT_NO_PARENT_FOUND");
    mesh_NO_PARENT_FOUND_handler(&event.info.no_parent);
    break;
	
   /*-- Mesh Network Update Events --*/	
   case MESH_EVENT_CHILD_CONNECTED:
    ESP_LOGW(TAG,"MESH_EVENT_CHILD_CONNECTED");
    mesh_CHILD_CONNECTED_handler(&event.info.child_connected);
    break;
   case MESH_EVENT_CHILD_DISCONNECTED:
    ESP_LOGW(TAG,"MESH_EVENT_CHILD_DISCONNECTED");
    mesh_CHILD_DISCONNECTED_handler(&event.info.child_disconnected);
    break;
   case MESH_EVENT_ROUTING_TABLE_ADD:
    ESP_LOGW(TAG,"MESH_EVENT_ROUTING_TABLE_ADD");
    mesh_ROUTING_TABLE_ADD_handler(&event.info.routing_table);
    break;
   case MESH_EVENT_ROUTING_TABLE_REMOVE:                                           
    ESP_LOGW(TAG,"MESH_EVENT_ROUTING_TABLE_REMOVE");
    mesh_ROUTING_TABLE_REMOVE_handler(&event.info.routing_table);
    break;
   case MESH_EVENT_ROOT_ADDRESS:
    ESP_LOGW(TAG,"MESH_EVENT_ROOT_ADDRESS");
    mesh_ROOT_ADDRESS_handler(&event.info.root_addr);
    break;
   case MESH_EVENT_ROOT_FIXED:
    ESP_LOGW(TAG,"MESH_EVENT_ROOT_FIXED");
    mesh_ROOT_FIXED_handler(&event.info.root_fixed);
    break;
   case MESH_EVENT_TODS_STATE:
    ESP_LOGW(TAG,"MESH_EVENT_TODS_STATE");
    mesh_TODS_STATE_handler(&event.info.toDS_state);
    break;
   case MESH_EVENT_VOTE_STARTED:
    ESP_LOGW(TAG,"MESH_EVENT_VOTE_STARTED");
    mesh_VOTE_STARTED_handler(&event.info.vote_started);
    break;
   case MESH_EVENT_VOTE_STOPPED:
    ESP_LOGW(TAG,"MESH_EVENT_VOTE_STOPPED");
    mesh_VOTE_STOPPED_handler();
    break;	
   case MESH_EVENT_LAYER_CHANGE:
    ESP_LOGW(TAG,"MESH_EVENT_LAYER_CHANGE");
    mesh_LAYER_CHANGE_handler(&event.info.layer_change);
    break;
   case MESH_EVENT_CHANNEL_SWITCH:
    ESP_LOGW(TAG,"MESH_EVENT_CHANNEL_SWITCH");
    mesh_CHANNEL_SWITCH_handler(&event.info.channel_switch);
    break;
  
   /*-- Mesh Root-specific Events --*/
   case MESH_EVENT_ROOT_GOT_IP:
    ESP_LOGW(TAG,"MESH_EVENT_ROOT_GOT_IP");
    mesh_ROOT_GOT_IP_handler(&event.info.got_ip);
    break;
   case MESH_EVENT_ROOT_LOST_IP:
    ESP_LOGW(TAG,"MESH_EVENT_ROOT_LOST_IP");
    mesh_ROOT_LOST_IP_handler();
    break;
   case MESH_EVENT_ROOT_SWITCH_REQ:
    ESP_LOGW(TAG,"MESH_EVENT_ROOT_SWITCH_REQ");
    mesh_ROOT_SWITCH_REQ_handler(&event.info.switch_req);
    break;
   case MESH_EVENT_ROOT_SWITCH_ACK:
    ESP_LOGW(TAG,"MESH_EVENT_ROOT_SWITCH_ACK");
    mesh_ROOT_SWITCH_ACK_handler();
    break;
   case MESH_EVENT_ROOT_ASKED_YIELD:
    ESP_LOGW(TAG,"MESH_EVENT_ROOT_ASKED_YIELD");
    mesh_ROOT_ASKED_YIELD_handler(&event.info.root_conflict);
    break;

   default:
    ESP_LOGE(TAG,"Unknown Mesh Event with ID: %u",event.id);
    break;
  }
 return;
}

/*************************************************************************
 *                       Mesh Initializer Function
 *************************************************************************/
esp_err_t mesh_init()
{
 static const char* TAG = "[mesh_init]";

 //Disable the default system logging relative to the Wi-Fi and mesh stacks (comment to debug)
 esp_log_level_set("event",ESP_LOG_NONE);   //Disable the system events default logging
 esp_log_level_set("wifi",ESP_LOG_NONE);    //Disable the Wi-Fi stack default logging
 esp_log_level_set("mesh",ESP_LOG_NONE);    //Disable the mesh stack default logging

 printf("\n");
 ESP_LOGI(TAG,"=========================== [Mesh Initializer Function] ===========================\n");

 ESP_LOGI(TAG,"Wi-Fi Stack Configuration");
 ESP_LOGI(TAG,"-----------------------------------------------------------------------------------"); 

 //[1/6]: Initialize the system flash storage
 esp_err_t ret = nvs_flash_init();
 if(ret == ESP_ERR_NVS_NO_FREE_PAGES)   //Error recovery attempt
  {
   ESP_ERROR_CHECK(nvs_flash_erase());
   ret = nvs_flash_init();
  }
 ESP_ERROR_CHECK(ret);
 ESP_LOGI(TAG,"[1/6] - System flash storage successfully initialized");

 //[2/6]: Initialize the TCP/LwIP Stack
 tcpip_adapter_init();
 ESP_LOGI(TAG,"[2/6] - TCP/LwIP Stack successfully initialized");

 //[3/6]: Devolve the Handling of Wi-Fi Events
 ESP_ERROR_CHECK(esp_event_loop_init(NULL,NULL));
 ESP_LOGI(TAG,"[3/6] - Handling of Wi-Fi Events devolved to the Mesh Stack");

 //[4/6]: Initialize the Wi-Fi Stack
 wifi_init_config_t initcfg = WIFI_INIT_CONFIG_DEFAULT();
 ESP_ERROR_CHECK(esp_wifi_init(&initcfg));
 ESP_LOGI(TAG,"[4/6] - Wi-Fi Stack successfully initialized");

 //[5/6]: Disable Station DHCP Client and SoftAP DHCP Server
 ESP_ERROR_CHECK(tcpip_adapter_dhcpc_stop(TCPIP_ADAPTER_IF_STA));
 ESP_ERROR_CHECK(tcpip_adapter_dhcps_stop(TCPIP_ADAPTER_IF_AP));
 ESP_LOGI(TAG,"[5/6] - Station DHCP Client and SoftAP DHCP Server disabled");

 //[6/6]: Enable the Wi-Fi Interface
 ESP_ERROR_CHECK(esp_wifi_start());
 ESP_LOGI(TAG,"[6/6] - Wi-Fi Interface Enabled\n");

 ESP_LOGI(TAG,"Mesh Stack Configuration");
 ESP_LOGI(TAG,"-----------------------------------------------------------------------------------"); 
 
 //[1/6]: Initialize the Mesh Stack
 ESP_ERROR_CHECK(esp_mesh_init());
 ESP_LOGI(TAG,"[1/6] - Mesh Stack successfully initialized");

 //[2/6]: Create the Mesh Event Group
 mesh_event_group = xEventGroupCreate();
 xEventGroupSetBits(mesh_event_group,MESH_VOTE);                 //Set the MESH_VOTE flag (active low, 0 = election in progress, 1 = no election)
 ESP_LOGI(TAG,"[2/6] - Mesh event group successfully created");

 //[3/6]: Base Mesh Configuration
 mesh_cfg_t mesh_config = {0};

 ESP_LOGI(TAG,"[3/6] - Base Mesh Configuration");

   //[3.1]: Mesh Network Identifier (MID)
   strToMAC(MESH_NETWORK_ID,mesh_config.mesh_id.addr);
   memcpy(mesh_state.mid,mesh_config.mesh_id.addr,6);
   ESP_LOGI(TAG,"  |--[3.1] - Mesh Network Identifier (MID): %s",MESH_NETWORK_ID);

   //[3.2]: Mesh Wi-Fi Channel
   mesh_config.channel = MESH_WIFI_CHANNEL;
   mesh_state.channel = mesh_config.channel;
   ESP_LOGI(TAG,"  |--[3.2] - Mesh Wi-Fi Channel: %i",MESH_WIFI_CHANNEL);

   //[3.3]: Mesh Events General Handler Address
   mesh_config.event_cb = &mesh_events_handler;
   ESP_LOGI(TAG,"  |--[3.3] - Mesh Events General Handler address set");

   //[3.4]: Base Mesh SoftAP Settings
   memcpy((uint8_t*)&mesh_config.mesh_ap.password,MESH_SOFTAP_PASSWORD,strlen(MESH_SOFTAP_PASSWORD));
   mesh_config.mesh_ap.max_connection = MESH_SOFTAP_MAXCONNECTIONS;
   ESP_LOGI(TAG,"  |--[3.4] - Mesh SoftAP settings: Password = %s, Max Connections = %u",MESH_SOFTAP_PASSWORD,MESH_SOFTAP_MAXCONNECTIONS);
 
   //[3.5]: Mesh Router Settings
   memcpy((uint8_t*)&mesh_config.router.ssid,MESH_ROUTER_SSID,strlen(MESH_ROUTER_SSID));
   mesh_config.router.ssid_len = strlen(MESH_ROUTER_SSID);
   memcpy((uint8_t*)&mesh_config.router.password,MESH_ROUTER_PASSWORD,strlen(MESH_ROUTER_PASSWORD));
   if(MESH_ROUTER_SPECIFIC_BSSID)    //Whether the root node of the network should attempt to connect to an external router with a specific BSSID only
    {
     strToMAC(MESH_ROUTER_BSSID,mesh_config.router.bssid);
     ESP_LOGI(TAG,"  |--[3.5] - Mesh Router settings: SSID = %s, Password = %s, specific BSSID = %s",MESH_ROUTER_SSID,MESH_ROUTER_PASSWORD,MESH_ROUTER_BSSID);
    }
   else
    ESP_LOGI(TAG,"  |--[3.5] - Mesh Router settings: SSID = %s, Password = %s",MESH_ROUTER_SSID,MESH_ROUTER_PASSWORD);   

   //[3.6]: Mesh IE Crypto Function
   if(MESH_CRYPTO_FUNC == MESH_CRYPTO_AES)
    {   
     mesh_config.crypto_funcs = &g_wifi_default_mesh_crypto_funcs;
     ESP_LOGI(TAG,"  |--[3.6] - Mesh IE Crypto Function: AES");
    }
   else
    {   
     mesh_config.crypto_funcs = 0;
     ESP_LOGI(TAG,"  |--[3.6] - Mesh IE Crypto Function: None");
    }

   //Apply the Base Mesh Configuration
   ESP_ERROR_CHECK(esp_mesh_set_config(&mesh_config));

 //[4/6]: Mesh Shared Settings
 ESP_LOGI(TAG,"[4/6] - Mesh Shared Settings");

   //[4.1]: Mesh Organization Mode
   switch(MESH_ORGANIZATION_MODE)
    {
     case SELF_ORGANIZED:
      mesh_state.org = SELF_ORGANIZED;
      ESP_LOGI(TAG,"  |--[4.1] - Mesh Organization Mode: SELF-ORGANIZED");
      break;
     case FIXED_ROOT:
      ESP_ERROR_CHECK(esp_mesh_fix_root(true));
      mesh_state.org = FIXED_ROOT;
      ESP_LOGI(TAG,"  |--[4.1] - Mesh Organization Mode: FIXED-ROOT");
      break;
     case MANUAL_NETWORKING:
      ESP_ERROR_CHECK(esp_mesh_set_self_organized(false,false));
      ESP_ERROR_CHECK(esp_mesh_fix_root(true));
      mesh_state.org = MANUAL_NETWORKING;
      ESP_LOGI(TAG,"  |--[4.1] - Mesh Organization Mode: MANUAL NETWORKING");
      break;
     default:
      ESP_LOGE(TAG,"  |--[4.1] - Unknown Mesh Organization Mode \"%u\", aborting.",MESH_ORGANIZATION_MODE);
      abort();
    }

   //[4.2]: Mesh Topology Settings
   ESP_LOGI(TAG,"  |--[4.2] - Mesh Topology Settings");

     //[4.2.1]: Mesh Max Layer
     ESP_ERROR_CHECK(esp_mesh_set_max_layer(MESH_MAX_LAYER));
     ESP_LOGI(TAG,"  |    |--[4.2.1] - Mesh maximum layer: %i",MESH_MAX_LAYER);
 
     //[4.2.2]: Mesh Max Capacity
     /** TODO BUG: This setting is currently disregarded by nodes in a mesh network */
     ESP_ERROR_CHECK(esp_mesh_set_capacity_num(MESH_MAX_CAPACITY));
     ESP_LOGI(TAG,"  |    |--[4.2.2] - Mesh maximum capacity: %i",MESH_MAX_CAPACITY);        

   //[4.3]: Mesh Self-Organized Root Settings
   ESP_LOGI(TAG,"  |--[4.3] - Mesh Self-Organized Root Settings");
   
     //[4.3.1]: Root Election Voting Percentage Threshold
     float root_election_threshold = atof(MESH_ROOT_ELECTION_THRESHOLD);  //Convert the root election voting percentage threshold menuconfig string to float

     if((root_election_threshold<=0.0)||(root_election_threshold>1.0))    //If the custom root election voting percentage threshold is out of bounds, use the default one instead (0.9)
      {
       ESP_LOGE(TAG,"  |    |--[4.3.1] - Invalid root election voting percentage threshold \"%f\", maintaining the default value \"%f\"",root_election_threshold,esp_mesh_get_vote_percentage());
       abort();
      }
     ESP_ERROR_CHECK(esp_mesh_set_vote_percentage(root_election_threshold));
     ESP_LOGI(TAG,"  |    |--[4.3.1] - Root election voting percentage threshold: %s",MESH_ROOT_ELECTION_THRESHOLD);

     //[4.3.2]: Root Election Rounds     
     mesh_attempts_t self_attempts;                           //Used to store the number of attempts of implicit actions in a self-organized mesh network

     self_attempts.scan = MESH_ROOT_ELECTION_MIN_ROUNDS;      //Set the minimum rounds in a root election (note that for the esp_mesh_set_attempts() function to return  
     self_attempts.vote = 15;                                 //successfully its other members must be initialized to valid values, where here their default values are used)
     self_attempts.fail = 120;                                                       
     self_attempts.monitor_ie = 10;
     ESP_ERROR_CHECK(esp_mesh_set_attempts(&self_attempts));
     ESP_LOGI(TAG,"  |    |--[4.3.2] - Root election minimum rounds: %i",MESH_ROOT_ELECTION_MIN_ROUNDS);

     //[4.3.3]: Root Node Self-Healing Delay
     ESP_ERROR_CHECK(esp_mesh_set_root_healing_delay(MESH_ROOT_HEALING_DELAY));
     ESP_LOGI(TAG,"  |    |--[4.3.3] - Root node self-healing delay: %ims",MESH_ROOT_HEALING_DELAY);

   //[4.4]: Other Root Node Settings
   ESP_LOGI(TAG,"  |--[4.4] - Other Root Node Settings");

     //[4.4.1]: Set the size of the root node's receiving queue for packets destined for the external DS (TODS queue)
     ESP_ERROR_CHECK(esp_mesh_set_xon_qsize(MESH_ROOT_TODS_QUEUE_SIZE));
     ESP_LOGI(TAG,"  |    |--[4.4.1] - Root TODS receiving queue size: %i",MESH_ROOT_TODS_QUEUE_SIZE);
    
     //[4.4.2]: Root Station IP Configuration (applied later in the MESH_EVENT_PARENT_CONNECTED specific handler for the actual root node only) 
     if(MESH_ROOT_IPDYNAMIC)                                  //If a dynamic IP configuration must be used for the root node's station interface
      ESP_LOGI(TAG,"  |    |--[4.4.2] - Root node configured to retrieve a dynamic IP configuration for its station interface once connected to the external router");
     else                                                     //If a dynamic IP configuration must be used for the root node's station interface
      {
       ESP_LOGI(TAG,"  |    |--[4.4.2] - Root node configured to apply the following static IP configuration to its station interface once connected to the external router:");
       ESP_LOGI(TAG,"  |                 IP Address = %s, Netmask = %s, Gateway = %s",MESH_ROOT_STATIC_IP,MESH_ROOT_STATIC_NETMASK,MESH_ROOT_STATIC_GATEWAY);
       ESP_LOGI(TAG,"  |                 Primary DNS = %s, Secondary DNS = %s",MESH_ROOT_STATIC_DNS_PRIMARY,MESH_ROOT_STATIC_DNS_SECONDARY);
      }

   //[4.5]: Mesh Additional SoftAP Settings
   ESP_LOGI(TAG,"  |--[4.5] - Mesh Additional SoftAP Settings");

     //[4.5.1]: Mesh SoftAP Authmode
     char mesh_auth_str[14];                                                     //Used to print the mesh SoftAP authmode as a string

     ESP_ERROR_CHECK(esp_mesh_set_ap_authmode(MESH_SOFTAP_AUTHMODE));
     authToStr(MESH_SOFTAP_AUTHMODE,mesh_auth_str);                              //Converts the mesh SoftAP authmode to string
     ESP_LOGI(TAG,"       |--[4.5.1] - Mesh SoftAP Authmode: %s",mesh_auth_str);

     //[4.5.2]: Mesh Children Disassociation Delay
     ESP_ERROR_CHECK(esp_mesh_set_ap_assoc_expire(MESH_CHILDREN_DISASSOCIATION_DELAY));
     ESP_LOGI(TAG,"       |--[4.5.2] - Mesh Children Disassociation Delay: %is",MESH_CHILDREN_DISASSOCIATION_DELAY);

 //[5/6]: Mesh Node-specific Settings
 ESP_LOGI(TAG,"[5/6] - Mesh Node-specific Settings");

   //[5.1]: Add the node into a Mesh Group (note that while in this example a node may be added to a single mesh group only, in general this is not the case)
   if(MESH_NODE_INGROUP)
    {
     mesh_addr_t group_id;    //Used to store the GID of the mesh group the node must be added into

     strToMAC(MESH_NODE_GROUP_ID,group_id.addr);
     ESP_ERROR_CHECK(esp_mesh_set_group_id(&group_id,1));
     ESP_LOGI(TAG,"  |--[5.1] - Node added into mesh group: %s",MESH_NODE_GROUP_ID);
    }
   else
    ESP_LOGI(TAG,"  |--[5.1] - Node not added into any mesh group");

   //[5.2]: Wi-Fi Interfaces Custom MAC Addresses
   uint8_t mac[6];

   if(MESH_STATION_USE_CUSTOM_MAC)  //If a custom MAC address is used for the node's station interface
    {
     ESP_ERROR_CHECK(strToMAC(MESH_STATION_CUSTOM_MAC,mac));
     ESP_ERROR_CHECK(esp_wifi_set_mac(ESP_IF_WIFI_STA,mac));     //Apply the Station custom MAC address
     ESP_LOGI(TAG,"  |--[5.2] - Node using the custom MAC address %s for its station interface",MESH_STATION_CUSTOM_MAC);
    }
   else
    {
     ESP_ERROR_CHECK(esp_wifi_get_mac(ESP_IF_WIFI_STA,mac));
     ESP_LOGI(TAG,"  |--[5.2] - Node using the default NIC MAC address "MACSTR" for its station interface",MAC2STR(mac));
    }
	
   if(MESH_SOFTAP_USE_CUSTOM_MAC)   //If a custom MAC address is used for the node's SoftAP interface
    {
     ESP_ERROR_CHECK(strToMAC(MESH_SOFTAP_CUSTOM_MAC,mac));
     ESP_ERROR_CHECK(esp_wifi_set_mac(ESP_IF_WIFI_AP,mac));      //Apply the SoftAP custom MAC address
     ESP_LOGI(TAG,"  |--[5.2] - Node using the custom MAC address %s for its SoftAP interface",MESH_SOFTAP_CUSTOM_MAC);
    }
   else
    {
     ESP_ERROR_CHECK(esp_wifi_get_mac(ESP_IF_WIFI_AP,mac));
     ESP_LOGI(TAG,"  |--[5.2] - Node using the default NIC MAC address "MACSTR" for its SoftAP interface",MAC2STR(mac));
    }

 //[6/6]: Enable Mesh Networking
 ESP_ERROR_CHECK(esp_mesh_start());
 ESP_LOGI(TAG,"[6/6] - Mesh Networking Enabled");

 ESP_LOGI(TAG,"========================= [End Mesh Initializer Function] =========================\n");       
 return ESP_OK;
}