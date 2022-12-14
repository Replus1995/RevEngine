#pragma once

#ifdef PINE_PLATFORM_WINDOWS
	#ifdef PINE_BUILD_DLL
		#define PINE_API __declspec(dllexport)
	#else
		#define PINE_API __declspec(dllimport)
	#endif
#else
	#error Pine only supprt windows!
#endif
