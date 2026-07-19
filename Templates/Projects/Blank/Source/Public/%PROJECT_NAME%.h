#pragma once

#ifdef NB_EDITOR_BUILD
	#ifdef _WIN32
		#define %PROJECT_NAME_API% __declspec(dllexport)
	#else
		#define %PROJECT_NAME_API% __attribute__((visibility("default")))
	#endif
#else
	#define %PROJECT_NAME_API%
#endif
