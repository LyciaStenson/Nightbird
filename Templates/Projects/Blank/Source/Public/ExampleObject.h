#pragma once

#include "%PROJECT_NAME%.h"

#include "Core/SceneObject.h"

class %PROJECT_NAME_API% ExampleObject : public Nightbird::Core::SceneObject
{
public:
	NB_TYPE()
	
	ExampleObject();
	~ExampleObject() = default;

	void EnterScene() override;
	void Tick(float delta) override;

	float m_ExampleVar = 1.0f;
};
