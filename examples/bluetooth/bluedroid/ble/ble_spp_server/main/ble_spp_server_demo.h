/*
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * DEFINES
 ****************************************************************************************
 */
//#define SUPPORT_HEARTBEAT
//#define SPP_DEBUG_MODE

#define spp_sprintf(s,...)         sprintf((char*)(s), ##__VA_ARGS__)
#define SPP_DATA_MAX_LEN           (512)
#define SPP_CMD_MAX_LEN            (20)
#define SPP_STATUS_MAX_LEN         (20)
#define SPP_DATA_BUFF_MAX_LEN      (2*1024)
///Attributes State Machine
enum{
    SPP_IDX_SVC,

    SPP_IDX_SPP_DATA_RECV_CHAR,
    SPP_IDX_SPP_DATA_RECV_VAL,

    SPP_IDX_SPP_DATA_NOTIFY_CHAR,
    SPP_IDX_SPP_DATA_NTY_VAL,
    SPP_IDX_SPP_DATA_NTF_CFG,

    SPP_IDX_SPP_COMMAND_CHAR,
    SPP_IDX_SPP_COMMAND_VAL,

    SPP_IDX_SPP_STATUS_CHAR,
    SPP_IDX_SPP_STATUS_VAL,
    SPP_IDX_SPP_STATUS_CFG,

#ifdef SUPPORT_HEARTBEAT
    SPP_IDX_SPP_HEARTBEAT_CHAR,
    SPP_IDX_SPP_HEARTBEAT_VAL,
    SPP_IDX_SPP_HEARTBEAT_CFG,
#endif

    SPP_IDX_NB,
};

uint8_t scan_sample[] ="{\"state\":{\"reported\":{\"ssid\": [{\"id\":1,\"sd\":\"Network1\",\"mc\":\"6C:72:20:76:25:11\",\"rs\":\"-43\",\"am\":0,\"ch\":1},\\
{\"id\":2,\"sd\":\"Network2\",\"mc\":\"6C:72:20:76:25:11\",\"rs\":\"-43\",\"am\":0,\"ch\":1},\\
{\"id\":3,\"sd\":\"Network3\",\"mc\":\"6C:72:20:76:25:11\",\"rs\":\"-43\",\"am\":0,\"ch\":1},\\
{\"id\":4,\"sd\":\"Network4\",\"mc\":\"6C:72:20:76:25:11\",\"rs\":\"-43\",\"am\":0,\"ch\":1},\\
{\"id\":5,\"sd\":\"Network5\",\"mc\":\"6C:72:20:76:25:11\",\"rs\":\"-43\",\"am\":0,\"ch\":1},\\
{\"id\":6,\"sd\":\"Network6\",\"mc\":\"6C:72:20:76:25:11\",\"rs\":\"-43\",\"am\":0,\"ch\":1},\\
{\"id\":7,\"sd\":\"Network7\",\"mc\":\"6C:72:20:76:25:11\",\"rs\":\"-43\",\"am\":0,\"ch\":1},\\
{\"id\":8,\"sd\":\"Network8\",\"mc\":\"6C:72:20:76:25:11\",\"rs\":\"-43\",\"am\":0,\"ch\":1},\\
{\"id\":9,\"sd\":\"Network9\",\"mc\":\"6C:72:20:76:25:11\",\"rs\":\"-43\",\"am\":0,\"ch\":1},\\
{\"id\":10,\"sd\":\"Network10\",\"mc\":\"6C:72:20:76:25:11\",\"rs\":\"-43\",\"am\":0,\"ch\":1},\\
{\"id\":11,\"sd\":\"Network11\",\"mc\":\"6C:72:20:76:25:11\",\"rs\":\"-43\",\"am\":0,\"ch\":1},\\
{\"id\":12,\"sd\":\"Network12\",\"mc\":\"6C:72:20:76:25:11\",\"rs\":\"-43\",\"am\":0,\"ch\":1},\\
{\"id\":13,\"sd\":\"Network13\",\"mc\":\"6C:72:20:76:25:11\",\"rs\":\"-43\",\"am\":0,\"ch\":1},\\
{\"id\":14,\"sd\":\"Network14\",\"mc\":\"6C:72:20:76:25:11\",\"rs\":\"-43\",\"am\":0,\"ch\":1},\\
{\"id\":15,\"sd\":\"Network15\",\"mc\":\"6C:72:20:76:25:11\",\"rs\":\"-43\",\"am\":0,\"ch\":1},\\
{\"id\":16,\"sd\":\"Network16\",\"mc\":\"6C:72:20:76:25:11\",\"rs\":\"-43\",\"am\":0,\"ch\":1},\\
{\"id\":17,\"sd\":\"Network17\",\"mc\":\"6C:72:20:76:25:11\",\"rs\":\"-43\",\"am\":0,\"ch\":1},\\
{\"id\":18,\"sd\":\"Network18\",\"mc\":\"6C:72:20:76:25:11\",\"rs\":\"-43\",\"am\":0,\"ch\":1},\\
{\"id\":19,\"sd\":\"Network19\",\"mc\":\"6C:72:20:76:25:11\",\"rs\":\"-43\",\"am\":0,\"ch\":1},\\
{\"id\":20,\"sd\":\"Network20\",\"mc\":\"6C:72:20:76:25:11\",\"rs\":\"-43\",\"am\":0,\"ch\":1},\\
{\"id\":21,\"sd\":\"Network21\",\"mc\":\"6C:72:20:76:25:11\",\"rs\":\"-43\",\"am\":0,\"ch\":1},\\
{\"id\":22,\"sd\":\"Network22\",\"mc\":\"6C:72:20:76:25:11\",\"rs\":\"-43\",\"am\":0,\"ch\":1}]}}}";
