#pragma once

#ifdef NB_EDITOR_BUILD
	#ifdef _WIN32
		#ifdef NB_ENGINE_BUILD
			#define NB_API __declspec(dllexport)
		#else
			#define NB_API __declspec(dllimport)
		#endif
	#else
		#ifdef NB_ENDING_BUILD
			#define NB_API __attribute__((visibility("default")))
		#else
			#define NB_API
		#endif
	#endif
#else
	#define NB_API
#endif
