#include "FileSystem.h"

#ifdef EUPHORBE_WINDOWS
	#include "Windows/WindowsFileSystem.h"
#endif

const char* E_GetAbsolutePath(const char* relative_path)
{
#ifdef EUPHORBE_WINDOWS
	return E_Window_GetAbsolutePath(relative_path);
#endif
}
