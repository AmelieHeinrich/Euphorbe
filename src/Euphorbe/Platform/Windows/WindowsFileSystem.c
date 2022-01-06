#include "WindowsFileSystem.h"

#include <Windows.h>

const char* E_Window_GetAbsolutePath(const char* relative_path)
{
    DWORD result = GetFullPathNameA(relative_path, 0, NULL, NULL);
    if (result)
    {
        char* buf = (char*)malloc(result);
        result = GetFullPathNameA(relative_path, result, buf, NULL);
        if (result == 0)
        {
            free(buf);
            return NULL;
        }
        return buf;
    }
    return NULL;
}