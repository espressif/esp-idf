/*************************************************************************************************************
 * Copyright © 2018 Riccardo Bertini <m_bertini@hotmail.com>                                                 *
 * Licensed under the Apache License, Version 2.0 (the "License");                                           *
 * you may not use this file except in compliance with the License.                                          *
 * You may obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0                        *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is      *
 * distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. *
 * See the License for the specific language governing permissions and limitations under the License.        *
 *************************************************************************************************************/

#ifndef MESHBASICS_DRIVER_MENUCONFIG_H
#define MESHBASICS_DRIVER_MENUCONFIG_H

/*-- esp_mesh_send() modes --*/
#define SEND_NONBLOCK        1
#define SEND_BLOCK           2

/*-- Sender Driver Packet Destinations --*/
#define SEND_PARENT          1
#define SEND_FIRSTCHILD      2
#define SEND_ALLCHILDREN     3
#define SEND_ALLDESCENDANTS  4
#define SEND_ROOT            5
#define SEND_SPECIFIC        6
#define SEND_GROUP           7
#define SEND_BROADCAST       8 

/*-- Root Waive Conditions --*/
#define WAIVE_RSSI           1
#define WAIVE_AUTOMATIC      2

/**================ [Logging Driver] ================**/
#ifdef CONFIG_MESH_DRIVER_LOGGING_ENABLE
 #define MESH_DRIVER_LOGGING_ENABLE 1
#else
 #define MESH_DRIVER_LOGGING_ENABLE 0
#endif

#ifdef CONFIG_MESH_DRIVER_LOGGING_EVENTGROUP
 #define MESH_DRIVER_LOGGING_EVENTGROUP 1
#else
 #define MESH_DRIVER_LOGGING_EVENTGROUP 0
#endif

#ifdef CONFIG_MESH_DRIVER_LOGGING_ROUTING
 #define MESH_DRIVER_LOGGING_ROUTING 1
#else
 #define MESH_DRIVER_LOGGING_ROUTING 0
#endif

#ifdef CONFIG_MESH_DRIVER_LOGGING_STATION
 #define MESH_DRIVER_LOGGING_STATION 1
#else
 #define MESH_DRIVER_LOGGING_STATION 0
#endif

#ifdef CONFIG_MESH_DRIVER_LOGGING_SOFTAP
 #define MESH_DRIVER_LOGGING_SOFTAP 1
#else
 #define MESH_DRIVER_LOGGING_SOFTAP 0
#endif

#ifdef CONFIG_MESH_DRIVER_LOGGING_INTERVAL
 #define MESH_DRIVER_LOGGING_INTERVAL CONFIG_MESH_DRIVER_LOGGING_INTERVAL
#else
 #define MESH_DRIVER_LOGGING_INTERVAL 0
#endif

/**================= [Sender Driver] =================**/
#ifdef CONFIG_MESH_DRIVER_SENDER_ENABLE
 #define MESH_DRIVER_SENDER_ENABLE 1
#else
 #define MESH_DRIVER_SENDER_ENABLE 0
#endif

#ifdef CONFIG_MESH_DRIVER_SENDER_DEST_PARENT 
 #define MESH_DRIVER_SENDER_DEST SEND_PARENT
#else
 #ifdef CONFIG_MESH_DRIVER_SENDER_DEST_FIRSTCHILD
  #define MESH_DRIVER_SENDER_DEST SEND_FIRSTCHILD
 #else
  #ifdef CONFIG_MESH_DRIVER_SENDER_DEST_ALLCHILDREN
   #define MESH_DRIVER_SENDER_DEST SEND_ALLCHILDREN
  #else
   #ifdef CONFIG_MESH_DRIVER_SENDER_DEST_ALLDESCENDANTS
    #define MESH_DRIVER_SENDER_DEST SEND_ALLDESCENDANTS
   #else
    #ifdef CONFIG_MESH_DRIVER_SENDER_DEST_ROOT
      #define MESH_DRIVER_SENDER_DEST SEND_ROOT
    #else
     #ifdef CONFIG_MESH_DRIVER_SENDER_DEST_SPECIFIC
      #define MESH_DRIVER_SENDER_DEST SEND_SPECIFIC
     #else
      #ifdef CONFIG_MESH_DRIVER_SENDER_DEST_GROUP
       #define MESH_DRIVER_SENDER_DEST SEND_GROUP
      #else
       #ifdef CONFIG_MESH_DRIVER_SENDER_DEST_BROADCAST     /** TODO BUG: Currently sending a mesh packet in broadcast causes the esp_mesh_send() function to return the ESP_ERR_MESH_NO_ROUTE_FOUND error */
        #define MESH_DRIVER_SENDER_DEST SEND_BROADCAST
       #else
        #define MESH_DRIVER_SENDER_DEST 0
       #endif
      #endif
     #endif
    #endif
   #endif
  #endif
 #endif
#endif

#ifdef CONFIG_MESH_DRIVER_SENDER_SPECIFIC_ADDR
 #define MESH_DRIVER_SENDER_SPECIFIC_ADDR CONFIG_MESH_DRIVER_SENDER_SPECIFIC_ADDR
#else
 #define MESH_DRIVER_SENDER_SPECIFIC_ADDR "0:0:0:0:0:0"
#endif

#ifdef CONFIG_MESH_DRIVER_SENDER_DEST_GROUP_ADDR
 #define MESH_DRIVER_SENDER_DEST_GROUP_ADDR CONFIG_MESH_DRIVER_SENDER_DEST_GROUP_ADDR
#else
 #define MESH_DRIVER_SENDER_DEST_GROUP_ADDR "0:0:0:0:0:0"
#endif

#ifdef CONFIG_MESH_DRIVER_SENDER_MESSAGE
 #define MESH_DRIVER_SENDER_MESSAGE CONFIG_MESH_DRIVER_SENDER_MESSAGE
#else
 #define MESH_DRIVER_SENDER_MESSAGE ""
#endif

#ifdef CONFIG_MESH_DRIVER_SENDER_MODE_BLOCKING
 #define MESH_DRIVER_SENDER_MODE SEND_BLOCK
#else
 #define MESH_DRIVER_SENDER_MODE SEND_NONBLOCK             /** TODO BUG: Currently using this sending mode still causes the esp_mesh_send() function to block if it would block in the first place     */ 
#endif                                                     /**           it should also be noted that the Mesh API currently don't offer any method to set a timeout for the esp_mesh_send() function) */

#ifdef CONFIG_MESH_DRIVER_SENDER_PROTOCOL_BIN
 #define MESH_DRIVER_SENDER_PROTOCOL MESH_PROTO_BIN
#else
 #ifdef CONFIG_MESH_DRIVER_SENDER_PROTOCOL_HTTP
  #define MESH_DRIVER_SENDER_PROTOCOL MESH_PROTO_HTTP
 #else
  #ifdef CONFIG_MESH_DRIVER_SENDER_PROTOCOL_JSON
   #define MESH_DRIVER_SENDER_PROTOCOL MESH_PROTO_JSON
  #else
   #define MESH_DRIVER_SENDER_PROTOCOL MESH_PROTO_MQTT
  #endif
 #endif
#endif

#ifdef CONFIG_MESH_DRIVER_SENDER_TOS_RELIABLE
 #define MESH_DRIVER_SENDER_TOS MESH_TOS_P2P
#else
 #define MESH_DRIVER_SENDER_TOS MESH_TOS_DEF               /** TODO BUG: Currently using the unreliable TOS causes the esp_mesh_send() function to fail if the packet's destination address */
#endif                                                     /**           is not found in the routing table of a node (packet that so would be forwarded upstream to the node's parent)      */
            
#ifdef CONFIG_MESH_DRIVER_SENDER_INTERVAL
 #define MESH_DRIVER_SENDER_INTERVAL CONFIG_MESH_DRIVER_SENDER_INTERVAL
#else
 #define MESH_DRIVER_SENDER_INTERVAL 0
#endif

/**=============== [Receiver Driver] ===============**/
#ifdef CONFIG_MESH_DRIVER_RECEIVER_ENABLE
 #define MESH_DRIVER_RECEIVER_ENABLE 1
#else
 #define MESH_DRIVER_RECEIVER_ENABLE 0
#endif

#ifdef CONFIG_MESH_DRIVER_RECEIVER_REPLY
 #define MESH_DRIVER_RECEIVER_REPLY 1
#else
 #define MESH_DRIVER_RECEIVER_REPLY 0
#endif

#ifdef CONFIG_MESH_DRIVER_RECEIVER_REPLY_MESSAGE
 #define MESH_DRIVER_RECEIVER_REPLY_MESSAGE CONFIG_MESH_DRIVER_RECEIVER_REPLY_MESSAGE
#else
 #define MESH_DRIVER_RECEIVER_REPLY_MESSAGE ""
#endif

#ifdef CONFIG_MESH_DRIVER_RECEIVER_REPLY_MODE_BLOCKING
 #define MESH_DRIVER_RECEIVER_REPLY_MODE SEND_BLOCK
#else
 #define MESH_DRIVER_RECEIVER_REPLY_MODE SEND_NONBLOCK     /** TODO BUG: Currently using this sending mode still causes the esp_mesh_send() function to block if it would block in the first place      */
#endif                                                     /**           (it should also be noted that the Mesh API currently don't offer any method to set a timeout for the esp_mesh_send() function) */         

#ifdef CONFIG_MESH_DRIVER_RECEIVER_REPLY_PROTOCOL_BIN
 #define MESH_DRIVER_RECEIVER_REPLY_PROTOCOL MESH_PROTO_BIN
#else
 #ifdef CONFIG_MESH_DRIVER_RECEIVER_REPLY_PROTOCOL_HTTP
  #define MESH_DRIVER_RECEIVER_REPLY_PROTOCOL MESH_PROTO_HTTP
 #else
  #ifdef CONFIG_MESH_DRIVER_RECEIVER_REPLY_PROTOCOL_JSON
   #define MESH_DRIVER_RECEIVER_REPLY_PROTOCOL MESH_PROTO_JSON
  #else
   #define MESH_DRIVER_RECEIVER_REPLY_PROTOCOL MESH_PROTO_MQTT
  #endif
 #endif
#endif

#ifdef CONFIG_MESH_DRIVER_RECEIVER_REPLY_TOS_RELIABLE
 #define MESH_DRIVER_RECEIVER_REPLY_TOS MESH_TOS_P2P
#else
 #define MESH_DRIVER_RECEIVER_REPLY_TOS MESH_TOS_DEF       /** TODO BUG: Currently using the unreliable TOS causes the esp_mesh_send() function to fail if the packet's destination address */
#endif                                                     /**           is not found in the routing table of a node (packet that so would be forwarded upstream to the node's parent)      */

/**================ [SenderDS Driver] ================**/
#ifdef CONFIG_MESH_DRIVER_SENDERDS_ENABLE
 #define MESH_DRIVER_SENDERDS_ENABLE 1
#else
 #define MESH_DRIVER_SENDERDS_ENABLE 0
#endif

#ifdef CONFIG_MESH_DRIVER_SENDERDS_DESTINATION_IP
 #define MESH_DRIVER_SENDERDS_DESTINATION_IP CONFIG_MESH_DRIVER_SENDERDS_DESTINATION_IP
#else
 #define MESH_DRIVER_SENDERDS_DESTINATION_IP "0.0.0.0"
#endif

#ifdef CONFIG_MESH_DRIVER_SENDERDS_DESTINATION_PORT
 #define MESH_DRIVER_SENDERDS_DESTINATION_PORT CONFIG_MESH_DRIVER_SENDERDS_DESTINATION_PORT
#else
 #define MESH_DRIVER_SENDERDS_DESTINATION_PORT 0
#endif

#ifdef CONFIG_MESH_DRIVER_SENDERDS_MESSAGE
 #define MESH_DRIVER_SENDERDS_MESSAGE CONFIG_MESH_DRIVER_SENDERDS_MESSAGE
#else
 #define MESH_DRIVER_SENDERDS_MESSAGE ""
#endif

#ifdef CONFIG_MESH_DRIVER_SENDERDS_MODE_BLOCKING
 #define MESH_DRIVER_SENDERDS_MODE SEND_BLOCK
#else
 #define MESH_DRIVER_SENDERDS_MODE SEND_NONBLOCK           /** TODO BUG: Currently using this sending mode still causes the esp_mesh_send() function to block if it would block in the first place      */
#endif                                                     /**           (it should also be noted that the Mesh API currently don't offer any method to set a timeout for the esp_mesh_send() function) */                    

#ifdef CONFIG_MESH_DRIVER_SENDERDS_PROTOCOL_BIN
 #define MESH_DRIVER_SENDERDS_PROTOCOL MESH_PROTO_BIN
#else
 #ifdef CONFIG_MESH_DRIVER_SENDERDS_PROTOCOL_HTTP
  #define MESH_DRIVER_SENDERDS_PROTOCOL MESH_PROTO_HTTP
 #else
  #ifdef CONFIG_MESH_DRIVER_SENDERDS_PROTOCOL_JSON
   #define MESH_DRIVER_SENDERDS_PROTOCOL MESH_PROTO_JSON
  #else
   #define MESH_DRIVER_SENDERDS_PROTOCOL MESH_PROTO_MQTT
  #endif
 #endif
#endif

#ifdef CONFIG_MESH_DRIVER_SENDERDS_TOS_RELIABLE
 #define MESH_DRIVER_SENDERDS_TOS MESH_TOS_P2P
#else
 #define MESH_DRIVER_SENDERDS_TOS MESH_TOS_DEF             /** TODO BUG: Currently using the unreliable TOS causes the esp_mesh_send() function to fail if the packet's destination address is not    */
#endif                                                     /**           found in the routing table of a node (packet that so would be forwarded upstream to the node's parent), and so if sending    */
                                                           /**           to an external host using an unreliable TOS causes the esp_mesh_send() function to fail on all nodes except on the root node */               
#ifdef CONFIG_MESH_DRIVER_SENDERDS_DROP
 #define MESH_DRIVER_SENDERDS_DROP 1                       /** TODO BUG: Currently the MESH_DATA_DROP bit in a packet's flag is either not actually set in the  */
#else                                                      /**           sent packet or in any case not received at the application level on the root node      */
 #define MESH_DRIVER_SENDERDS_DROP 0
#endif

#ifdef CONFIG_MESH_DRIVER_SENDERDS_INTERVAL
 #define MESH_DRIVER_SENDERDS_INTERVAL CONFIG_MESH_DRIVER_SENDERDS_INTERVAL
#else
 #define MESH_DRIVER_SENDERDS_INTERVAL 0
#endif

/**============== [ReceiverDS Driver] ==============**/
#ifdef CONFIG_MESH_DRIVER_RECEIVERDS_ENABLE
 #define MESH_DRIVER_RECEIVERDS_ENABLE 1
#else
 #define MESH_DRIVER_RECEIVERDS_ENABLE 0
#endif

/**================= [Waive Driver] =================**/
#ifdef CONFIG_MESH_DRIVER_WAIVE_ENABLE
 #define MESH_DRIVER_WAIVE_ENABLE 1
#else
 #define MESH_DRIVER_WAIVE_ENABLE 0
#endif

#ifdef CONFIG_MESH_DRIVER_WAIVE_VOTING_THRESHOLD
 #define MESH_DRIVER_WAIVE_VOTING_THRESHOLD CONFIG_MESH_DRIVER_WAIVE_VOTING_THRESHOLD
#else
 #define MESH_DRIVER_WAIVE_VOTING_THRESHOLD "1.0"
#endif

#ifdef CONFIG_MESH_DRIVER_WAIVE_ELECTION_MAX_ROUNDS
 #define MESH_DRIVER_WAIVE_ELECTION_MAX_ROUNDS CONFIG_MESH_DRIVER_WAIVE_ELECTION_MAX_ROUNDS
#else
 #define MESH_DRIVER_WAIVE_ELECTION_MAX_ROUNDS 15
#endif

#ifdef CONFIG_MESH_DRIVER_WAIVE_RSSI
 #define MESH_DRIVER_WAIVE_CONDITION WAIVE_RSSI
#else
 #define MESH_DRIVER_WAIVE_CONDITION WAIVE_AUTOMATIC
#endif

#ifdef CONFIG_MESH_DRIVER_WAIVE_RSSI_THRESHOLD
 #define MESH_DRIVER_WAIVE_RSSI_THRESHOLD CONFIG_MESH_DRIVER_WAIVE_RSSI_THRESHOLD
#else
 #define MESH_DRIVER_WAIVE_RSSI_THRESHOLD 0
#endif

#ifdef CONFIG_MESH_DRIVER_WAIVE_RSSI_INTERVAL
 #define MESH_DRIVER_WAIVE_RSSI_INTERVAL CONFIG_MESH_DRIVER_WAIVE_RSSI_INTERVAL
#else
 #define MESH_DRIVER_WAIVE_RSSI_INTERVAL 0
#endif

#ifdef CONFIG_MESH_DRIVER_WAIVE_AUTOMATIC_INTERVAL
 #define MESH_DRIVER_WAIVE_AUTOMATIC_INTERVAL CONFIG_MESH_DRIVER_WAIVE_AUTOMATIC_INTERVAL
#else
 #define MESH_DRIVER_WAIVE_AUTOMATIC_INTERVAL 0
#endif

#endif /*-- MESHBASICS_DRIVER_MENUCONFIG_H --*/