#include "Core/PointLight.h"

NB_REFLECT(Nightbird::Core::PointLight, NB_PARENT(Nightbird::Core::SpatialObject), NB_FACTORY(Nightbird::Core::PointLight),
	NB_FIELD(m_Color),
	NB_FIELD(m_Intensity),
	NB_FIELD(m_Radius)
)
