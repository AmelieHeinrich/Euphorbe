#include "Input.h"

#ifdef EUPHORBE_WINDOWS
	#include <Euphorbe/Platform/Windows/WindowsInput.h>
#endif


b32 E_IsKeyPressed(u16 key)
{
#ifdef EUPHORBE_WINDOWS
	return E_WindowsIsKeyPressed(key);
#endif
}

b32 E_IsMouseButtonPressed(u16 button)
{
#ifdef EUPHORBE_WINDOWS
	return E_WindowsIsMouseButtonPressed(button);
#endif
}

f32 E_GetMousePosX()
{
#ifdef EUPHORBE_WINDOWS
	return E_WindowsGetMousePosX();
#endif
}

f32 E_GetMousePosY()
{
#ifdef EUPHORBE_WINDOWS
	return E_WindowsGetMousePosY();
#endif
}
