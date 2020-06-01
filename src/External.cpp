#ifdef WIN32
#include <Windows.h>
#elif __linux__
#include <unistd.h>
#include <bits/types/struct_timespec.h>
#include <time.h>

#endif
void SleepFunc(unsigned long milliseconds) {
#ifdef WIN32
	Sleep(milliseconds);
#elif _POSIX_C_SOURCE >= 199309L
	timespec ts;
	ts.tv_sec = milliseconds / 1000;
	ts.tv_nsec = (milliseconds % 1000) * 1000000;
	nanosleep(&ts, NULL);
#else
	usleep(milliseconds * 1000);
#endif
}