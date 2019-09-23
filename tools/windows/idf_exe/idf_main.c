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

#include <windows.h>
#include <shlwapi.h>
#include <strsafe.h>
#include <stdarg.h>

#define LINESIZE 1024

static void fail(LPCSTR message, ...) __attribute__((noreturn));

static void fail(LPCSTR message, ...)
{
    DWORD written;
    char msg[LINESIZE];
    va_list args = NULL;
    va_start(args, message);
    StringCchVPrintfA(msg, sizeof(msg), message, args);
    WriteFile(GetStdHandle(STD_ERROR_HANDLE), message, lstrlen(msg), &written, NULL);
    ExitProcess(1);
}

int main(int argc, LPTSTR argv[])
{
    /* Print the version of this wrapper tool, but only if invoked as "idf.exe".
     * "idf -v" will invoke idf.py as expected.
     */

    LPCTSTR cmdname = PathFindFileName(argv[0]);
    int cmdname_length = strlen(cmdname);

    if (argc == 2 &&
        cmdname_length > 4 &&
        StrCmp(cmdname + cmdname_length - 4, TEXT(".exe")) == 0 &&
            (StrCmp(argv[1], TEXT("--version")) == 0 ||
             StrCmp(argv[1], TEXT("-v")) == 0)) {
        LPCSTR msg = VERSION "\n";
        DWORD written;
        WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), msg, lstrlen(msg), &written, NULL);
        return 0;
    }

    LPCTSTR idfpy_script_name = TEXT("idf.py");

    /* Get IDF_PATH */
    TCHAR idf_path[LINESIZE] = {};
    if (GetEnvironmentVariable(TEXT("IDF_PATH"), idf_path, sizeof(idf_path)) == 0) {
        DWORD err = GetLastError();
        if (err == ERROR_ENVVAR_NOT_FOUND) {
            fail("IDF_PATH environment variable needs to be set to use this tool\n");
        } else {
            fail("Unknown error (%u)\n", err);
        }
    }

    /* Prepare the command line: python.exe %IDF_PATH%\\tools\idf.py <rest of the args> */
    TCHAR cmdline[LINESIZE] = {};
    StringCchCat(cmdline, sizeof(cmdline), TEXT("python.exe "));
    StringCchCat(cmdline, sizeof(cmdline), idf_path);
    StringCchCat(cmdline, sizeof(cmdline), TEXT("\\tools\\"));
    StringCchCat(cmdline, sizeof(cmdline), idfpy_script_name);
    StringCchCat(cmdline, sizeof(cmdline), TEXT(" "));
    for (int i = 1; i < argc; ++i) {
        StringCchCat(cmdline, sizeof(cmdline), argv[i]);
        StringCchCat(cmdline, sizeof(cmdline), TEXT(" "));
    }

    SetEnvironmentVariable(TEXT("IDF_PY_PROGRAM_NAME"), idfpy_script_name);

    /* Reuse the standard streams of this process */
    STARTUPINFO start_info = {
        .cb = sizeof(STARTUPINFO),
        .hStdError = GetStdHandle(STD_ERROR_HANDLE),
        .hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE),
        .hStdInput = GetStdHandle(STD_INPUT_HANDLE),
        .dwFlags = STARTF_USESTDHANDLES
    };

    /* Run the child process */
    PROCESS_INFORMATION child_process;
    if (!CreateProcess(NULL, cmdline, NULL, NULL, TRUE, 0, NULL, NULL, &start_info, &child_process)) {
        DWORD err = GetLastError();
        if (err == ERROR_FILE_NOT_FOUND) {
            fail("Can not find Python\n");
        } else {
            fail("Unknown error (%u)\n", err);
        }
    }

    /* Wait for it to complete */
    WaitForSingleObject(child_process.hProcess, INFINITE);

    /* Return with the exit code of the child process */
    DWORD exitcode;
    if (!GetExitCodeProcess(child_process.hProcess, &exitcode)) {
        fail("Couldn't get the exit code (%u)\n", GetLastError());
    }
    return exitcode;
}
