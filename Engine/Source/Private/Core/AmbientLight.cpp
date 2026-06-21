#include "Core/AmbientLight.h"

NB_REFLECT(Nightbird::Core::AmbientLight, NB_PARENT(Nightbird::Core::SceneObject), NB_FACTORY(Nightbird::Core::AmbientLight),
	NB_FIELD(m_Color),
	NB_FIELD(m_Intensity)
)
