#ifndef EUPHORBE_WINDOWS_TIMER_H
#define EUPHORBE_WINDOWS_TIMER_H

#include <Euphorbe/Platform/Timer.h>

typedef struct E_WindowsTimer E_WindowsTimer;
struct E_WindowsTimer
{
	i64 start;
	f64 frequency;
};

void E_WindowsTimerInit();
f32 E_WindowsTimerGetTime();

#endif