/* Console example — various system commands

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Register all system functions
void register_system(void);

// Register common system functions: "version", "restart", "free", "heap", "tasks"
void register_system_common(void);

// Register deep and light sleep functions
void register_system_deep_sleep(void);
void register_system_light_sleep(void);

// Register common tools used in shell: cat, echo, grep, tail, tee
void register_system_shell_common(void);

void register_system_shell_tee(void);
void register_system_shell_cat(void);
void register_system_shell_grep(void);
void register_system_shell_echo(void);

#ifdef __cplusplus
}
#endif
