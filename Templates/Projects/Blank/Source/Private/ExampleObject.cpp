#include "ExampleObject.h"

NB_REFLECT(ExampleObject, NB_PARENT(Nightbird::Core::SceneObject), NB_FACTORY(ExampleObject),
	NB_FIELD(m_ExampleVar)
)

ExampleObject::ExampleObject()
	: SceneObject()
{

}

void ExampleObject::EnterScene()
{

}

void ExampleObject::Tick(float delta)
{

}
