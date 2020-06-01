#pragma once
#ifdef WIN32
#ifdef __cplusplus
extern "C" {
#endif
__declspec(dllimport) int __stdcall QueryPerformanceCounter(unsigned long long int *lpPerformanceCount);
__declspec(dllimport) int __stdcall QueryPerformanceFrequency(unsigned long long int *lpFrequency);
#ifdef __cplusplus
}
#endif
#endif