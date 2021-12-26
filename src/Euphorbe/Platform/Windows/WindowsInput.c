#include "WindowsInput.h"

b32 E_WindowsIsKeyPressed(u16 key)
{
	SHORT state = GetAsyncKeyState((int)key);
	return state & 0x8000;
}

b32 E_WindowsIsMouseButtonPressed(u16 btn)
{
	SHORT state = GetAsyncKeyState((int)btn);
	return state & 0x8000;
}

f32 E_WindowsGetMousePosX()
{
	POINT p;
	GetCursorPos(&p);
	return p.x;
}

f32 E_WindowsGetMousePosY()
{
	POINT p;
	GetCursorPos(&p);
	return p.y;
}
