/* Console example — various system commands

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/


#include "cmd_system.h"
#include "sdkconfig.h"

void register_system(void)
{
    register_system_common();
#ifndef CONFIG_IDF_TARGET_ESP32H2  // IDF-6268
    register_system_sleep();
#endif
}
