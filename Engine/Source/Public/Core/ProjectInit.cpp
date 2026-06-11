#include "Core/TypeRegistry.h"

#ifdef _WIN32
	#define NB_EXPORT extern "C" __declspec(dllexport)
#else
	#define NB_EXPORT extern "C" __attribute__((visibility("default")))
#endif

NB_EXPORT void NB_InitProject(Nightbird::RegisterTypeFn registerType)
{
	// Run Project's apply list for local TypeRegistry
	Nightbird::TypeRegistry::InitReflection();
	
	const auto& types = Nightbird::TypeRegistry::GetAll();

	// Push locally registered types into Editor's registry
	for (uint32_t i = 0; i < static_cast<uint32_t>(types.size()); ++i)
		registerType(types[i]);
}