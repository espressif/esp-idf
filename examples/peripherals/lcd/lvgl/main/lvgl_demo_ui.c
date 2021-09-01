/* LCD LVGL UI example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include "lvgl.h"

void example_lvgl_demo_ui(lv_obj_t *scr)
{
    // Create a Label on the currently active screen
    lv_obj_t *label =  lv_label_create(scr);
    // Modify the Label's text
    lv_label_set_text(label, "Hello World");
    // Align the Label to the center
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);
}
