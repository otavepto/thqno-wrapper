#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>

#ifdef THQNOWRAPPERSR_EXPORTS
	#define API extern "C" __declspec(dllexport) 
#else
	#define API extern "C" __declspec(dllimport) 
#endif