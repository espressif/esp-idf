/* Mesh Internal Communication Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#ifndef __MESH_LIGHT_H__
#define __MESH_LIGHT_H__

#include "esp_err.h"

/*******************************************************
 *                Constants
 *******************************************************/
#define MESH_LIGHT_RED       (0xff)
#define MESH_LIGHT_GREEN     (0xfe)
#define MESH_LIGHT_BLUE      (0xfd)
#define MESH_LIGHT_YELLOW    (0xfc)
#define MESH_LIGHT_PINK      (0xfb)
#define MESH_LIGHT_INIT      (0xfa)
#define MESH_LIGHT_WARNING   (0xf9)

#define  MESH_TOKEN_ID       (0x0)
#define  MESH_TOKEN_VALUE    (0xbeef)
#define  MESH_CONTROL_CMD    (0x2)

/*******************************************************
 *                Type Definitions
 *******************************************************/

/*******************************************************
 *                Structures
 *******************************************************/
typedef struct {
    uint8_t cmd;
    bool on;
    uint8_t token_id;
    uint16_t token_value;
} mesh_light_ctl_t;

/*******************************************************
 *                Variables Declarations
 *******************************************************/

/*******************************************************
 *                Function Definitions
 *******************************************************/
esp_err_t mesh_light_init(void);
esp_err_t mesh_light_set(int color);
esp_err_t mesh_light_process(mesh_addr_t *from, uint8_t *buf, uint16_t len);
void mesh_connected_indicator(int layer);
void mesh_disconnected_indicator(void);

#endif /* __MESH_LIGHT_H__ */
