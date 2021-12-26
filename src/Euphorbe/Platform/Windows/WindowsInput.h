#ifndef EUPHORBE_WINDOWS_INPUT_H
#define EUPHORBE_WINDOWS_INPUT_H

#include <Euphorbe/Platform/Input.h>

b32 E_WindowsIsKeyPressed(u16 key);
b32 E_WindowsIsMouseButtonPressed(u16 btn);
f32 E_WindowsGetMousePosX();
f32 E_WindowsGetMousePosY();

#endif