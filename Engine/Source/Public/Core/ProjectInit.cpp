#include "Core/TypeRegistry.h"

#ifdef _WIN32
	#define NB_EXPORT extern "C" __declspec(dllexport)
#else
	#define NB_EXPORT extern "C" __attribute__((visibility("default")))
#endif

NB_EXPORT void NB_InitProject(Nightbird::RegisterTypeFn registerType)
{
	Nightbird::TypeRegistry::InitReflection();
}
