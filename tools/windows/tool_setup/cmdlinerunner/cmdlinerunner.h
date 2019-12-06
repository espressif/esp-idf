// Copyright 2019 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at",
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License

#pragma once

#include <windows.h>

struct proc_instance_s;
typedef struct proc_instance_s proc_instance_t;

#ifdef CMDLINERUNNER_EXPORTS
#define CMDLINERUNNER_API __declspec(dllexport)
#else
#define CMDLINERUNNER_API __declspec(dllimport)
#endif

CMDLINERUNNER_API proc_instance_t * proc_start(LPCTSTR cmdline, LPCTSTR workdir);
CMDLINERUNNER_API int proc_get_exit_code(proc_instance_t *inst);
CMDLINERUNNER_API DWORD proc_get_output(proc_instance_t *inst, LPSTR dest, DWORD sz);
CMDLINERUNNER_API void proc_end(proc_instance_t *inst);
CMDLINERUNNER_API BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved );
