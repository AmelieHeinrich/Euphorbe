#include "WindowsTimer.h"

#define SECONDS_PER_FRAME(start_counter, end_counter, frequency) ((f32)(end_counter.QuadPart - start_counter.QuadPart) / (f32)frequency.QuadPart)

E_WindowsTimer timer;

void E_WindowsTimerInit()
{
	LARGE_INTEGER large;

	QueryPerformanceCounter(&large);
	timer.start = large.QuadPart;

	QueryPerformanceFrequency(&large);
	timer.frequency = (f64)large.QuadPart;
}

f32 E_WindowsTimerGetTime()
{
	LARGE_INTEGER large_int;
	QueryPerformanceCounter(&large_int);

	i64 now = large_int.QuadPart;
	i64 time = now - timer.start;

	return (f32)((f64)time / timer.frequency);
}
