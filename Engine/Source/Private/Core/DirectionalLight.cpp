#include "Core/DirectionalLight.h"

NB_REFLECT(Nightbird::Core::DirectionalLight, NB_PARENT(Nightbird::Core::SpatialObject), NB_FACTORY(Nightbird::Core::DirectionalLight),
	NB_FIELD(m_Color),
	NB_FIELD(m_Intensity)
)
