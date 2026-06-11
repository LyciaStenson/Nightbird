#include "Core/SceneObject.h"

#include "Core/Engine.h"
#include "Core/Scene.h"
#include "Core/Log.h"

NB_REFLECT(Nightbird::Core::SceneObject, NB_NO_PARENT, NB_FACTORY(Nightbird::Core::SceneObject),
	NB_FIELD(m_Name)
)

namespace Nightbird::Core
{
	SceneObject::SceneObject()
		: m_Name("SceneObject")
	{

	}

	const std::string& SceneObject::GetName() const
	{
		return m_Name;
	}

	void SceneObject::SetName(std::string name)
	{
		m_Name = std::move(name);
	}

	Engine* SceneObject::GetEngine() const
	{
		return m_Scene ? m_Scene->GetEngine() : nullptr;
	}

	void SceneObject::SetScene(Scene* scene)
	{
		m_Scene = scene;
		for (auto& child : m_Children)
			child->SetScene(scene);
	}

	SceneObject* SceneObject::GetParent() const
	{
		return m_Parent;
	}

	void SceneObject::SetParent(SceneObject* newParent)
	{
		if (newParent == m_Parent || newParent == this)
			return;

		std::unique_ptr<SceneObject> detachedChild = nullptr;
		if (m_Parent)
			detachedChild = m_Parent->DetachChild(this);

		m_Parent = newParent;

		if (newParent && detachedChild)
			newParent->AddChild(std::move(detachedChild));
	}

	const std::vector<std::unique_ptr<SceneObject>>& SceneObject::GetChildren() const
	{
		return m_Children;
	}

	std::vector<std::unique_ptr<SceneObject>>& SceneObject::GetChildren()
	{
		return m_Children;
	}

	void SceneObject::AddChild(std::unique_ptr<SceneObject> child)
	{
		if (!child)
			return;

		m_Children.push_back(std::move(child));
		
		m_Children.back()->SetScene(m_Scene);
		m_Children.back()->SetParent(this);

		if (m_Scene && m_Scene->GetEngine())
			m_Children.back()->EnterSceneRecursive();
	}

	std::unique_ptr<SceneObject> SceneObject::DetachChild(SceneObject* child)
	{
		for (auto it = m_Children.begin(); it != m_Children.end(); ++it)
		{
			if (it->get() == child)
			{
				auto detatched = std::move(*it);
				m_Children.erase(it);
				detatched->SetParent(nullptr);
				return detatched;
			}
		}

		return nullptr;
	}

	bool SceneObject::HasSourceScene() const
	{
		return m_SourceSceneUUID.has_value();
	}

	const std::optional<uuids::uuid>& SceneObject::GetSourceSceneUUID() const
	{
		return m_SourceSceneUUID;
	}

	void SceneObject::SetSourceSceneUUID(const uuids::uuid& uuid)
	{
		m_SourceSceneUUID = uuid;
	}

	void SceneObject::EnterSceneRecursive()
	{
		EnterScene();
		for (const auto& child : m_Children)
			child->EnterSceneRecursive();
	}

	void SceneObject::EnterScene()
	{

	}

	void SceneObject::Tick(float delta)
	{

	}
}
