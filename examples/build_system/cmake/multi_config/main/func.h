/* Multiple build configurations example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief This function has different implementations depending on the product type.
 *
 * See func_dev.c and func_prod.c. Which of the files is compiled is determined in
 * CMakeLists.txt,
 */
void func(void);

#ifdef __cplusplus
}
#endif
