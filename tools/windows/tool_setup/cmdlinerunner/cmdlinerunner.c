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

#define CMDLINERUNNER_EXPORTS

#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include "cmdlinerunner.h"

#define LINESIZE 1024

#ifdef WITH_DEBUG
#include <stdio.h>
#define DEBUGV(...) do { fprintf(stderr, __VA_ARG__); } while(0)
#else
#define DEBUGV(...)
#endif

struct proc_instance_s {
    PROCESS_INFORMATION child_process;
    HANDLE pipe_server_handle;
    HANDLE pipe_client_handle;
};

#ifdef WITH_DEBUG
static void print_last_error()
{
    DWORD dw;
    TCHAR errmsg[LINESIZE];
    dw = GetLastError();

    FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL, dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        errmsg, sizeof(errmsg) - 1, NULL );
    DEBUGV("error %d: %s\n", dw, errmsg);
}
#define PRINT_LAST_ERROR() print_last_error()
#else
#define PRINT_LAST_ERROR()
#endif

static proc_instance_t *proc_instance_allocate()
{
    return (proc_instance_t*) HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(proc_instance_t));
}

static void proc_instance_free(proc_instance_t *instance)
{
    if (instance->pipe_server_handle) {
        CloseHandle(instance->pipe_server_handle);
    }
    if (instance->pipe_client_handle) {
        CloseHandle(instance->pipe_client_handle);
    }
    if (instance->child_process.hProcess) {
        TerminateProcess(instance->child_process.hProcess, 1);
        CloseHandle(instance->child_process.hProcess);
        CloseHandle(instance->child_process.hThread);
    }
    HeapFree(GetProcessHeap(), 0, instance);
}

void proc_end(proc_instance_t *inst)
{
    if (inst == NULL) {
        return;
    }
    proc_instance_free(inst);
}

CMDLINERUNNER_API proc_instance_t * proc_start(LPCTSTR cmdline, LPCTSTR workdir)
{
    proc_instance_t *inst = proc_instance_allocate();
    if (inst == NULL) {
        return NULL;
    }

    SECURITY_ATTRIBUTES sec_attr = {
        .nLength = sizeof(SECURITY_ATTRIBUTES),
        .bInheritHandle = TRUE,
        .lpSecurityDescriptor = NULL
    };

    LPCTSTR pipename = TEXT("\\\\.\\pipe\\cmdlinerunner_pipe");

    inst->pipe_server_handle = CreateNamedPipe(pipename, PIPE_ACCESS_DUPLEX,
            PIPE_TYPE_BYTE | PIPE_WAIT, 1, 1024 * 16, 1024 * 16,
            NMPWAIT_WAIT_FOREVER, &sec_attr);
    if (inst->pipe_server_handle == INVALID_HANDLE_VALUE) {
        DEBUGV("inst->pipe_server_handle == INVALID_HANDLE_VALUE\n");
        goto error;
    }

    inst->pipe_client_handle = CreateFile(pipename, GENERIC_WRITE | GENERIC_READ,
            0, &sec_attr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (inst->pipe_client_handle == INVALID_HANDLE_VALUE) {
        DEBUGV("inst->pipe_client_handle == INVALID_HANDLE_VALUE\n");
        goto error;
    }

    DWORD new_mode = PIPE_READMODE_BYTE | PIPE_NOWAIT;
    if (!SetNamedPipeHandleState(inst->pipe_server_handle, &new_mode, NULL,
            NULL)) {
        DEBUGV("SetNamedPipeHandleState failed\n");
        goto error;
    }

    if (!SetHandleInformation(inst->pipe_server_handle, HANDLE_FLAG_INHERIT, 0)) {
        DEBUGV("SetHandleInformation failed\n");
        goto error;
    }

    if (!SetHandleInformation(inst->pipe_client_handle, HANDLE_FLAG_INHERIT,
            HANDLE_FLAG_INHERIT)) {
        DEBUGV("SetHandleInformation failed\n");
        goto error;
    }

    STARTUPINFO siStartInfo = {
        .cb = sizeof(STARTUPINFO),
        .hStdError = inst->pipe_client_handle,
        .hStdOutput = inst->pipe_client_handle,
        .hStdInput = inst->pipe_client_handle,
        .dwFlags = STARTF_USESTDHANDLES
    };

    size_t workdir_len = 0;
    StringCbLength(workdir, STRSAFE_MAX_CCH * sizeof(TCHAR), &workdir_len);
    if (workdir_len == 0) {
        workdir = NULL;
    }

    TCHAR cmdline_tmp[LINESIZE];
    StringCbCopy(cmdline_tmp, sizeof(cmdline_tmp), cmdline);
    if (!CreateProcess(NULL, cmdline_tmp,
    NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, workdir, &siStartInfo,
            &inst->child_process)) {
        DEBUGV("CreateProcess failed\n");
        goto error;
    }
    return inst;

error:
    PRINT_LAST_ERROR();
    proc_instance_free(inst);
    return NULL;
}

int proc_get_exit_code(proc_instance_t *inst)
{
    DWORD result;
    if (!GetExitCodeProcess(inst->child_process.hProcess, &result)) {
        return -2;
    }
    if (result == STILL_ACTIVE) {
        return -1;
    }
    return (int) result;
}

DWORD proc_get_output(proc_instance_t *inst, LPSTR dest, DWORD sz)
{
    DWORD read_bytes;
    BOOL res = ReadFile(inst->pipe_server_handle, dest,
            sz - 1, &read_bytes, NULL);
    if (!res) {
        if (GetLastError() == ERROR_NO_DATA) {
            return 0;
        } else {
            PRINT_LAST_ERROR();
            return 0;
        }
    }
    dest[read_bytes] = 0;
    return read_bytes;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved )
{
    return TRUE;
}

