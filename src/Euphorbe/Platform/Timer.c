#include "Timer.h"

#ifdef EUPHORBE_WINDOWS
	#include "Windows/WindowsTimer.h"
#endif

void E_TimerInit()
{
#ifdef EUPHORBE_WINDOWS
	E_WindowsTimerInit();
#endif
}

f32 E_TimerGetTime()
{
#ifdef EUPHORBE_WINDOWS
	return E_WindowsTimerGetTime();
#endif
}
