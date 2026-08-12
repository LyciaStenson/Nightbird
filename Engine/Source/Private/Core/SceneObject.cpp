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

	void SceneObject::SetParent(SceneObject* newParent, int insertIndex)
	{
		if (newParent == this)
			return;
		
		SceneObject* oldParent = m_Parent;
		if (newParent == oldParent)
		{
			if (oldParent)
				oldParent->ReorderChild(this, insertIndex);
			return;
		}
		
		std::unique_ptr<SceneObject> detachedChild = nullptr;
		if (m_Parent)
			detachedChild = m_Parent->DetachChild(this);

		m_Parent = nullptr;

		if (newParent && detachedChild)
			newParent->AddChild(std::move(detachedChild), insertIndex);
	}
	
	void SceneObject::AddChild(std::unique_ptr<SceneObject> child, int insertIndex)
	{
		if (!child)
			return;

		SceneObject* newChild = child.get();
		
		if (insertIndex < 0 || insertIndex >= static_cast<int>(m_Children.size()))
			m_Children.push_back(std::move(child));
		else
			m_Children.insert(m_Children.begin() + insertIndex, std::move(child));

		newChild->m_Parent = this;
		newChild->SetScene(m_Scene);
		
		if (m_Scene && m_Scene->GetEngine())
			newChild->EnterSceneRecursive();
	}

	void SceneObject::ReorderChild(SceneObject* child, int insertIndex)
	{
		if (!child || insertIndex < 0)
			return;

		auto it = std::find_if(m_Children.begin(), m_Children.end(),
			[child](const std::unique_ptr<SceneObject>& c)
			{
				return c.get() == child;
			});
		if (it == m_Children.end())
			return;

		int currentIndex = static_cast<int>(std::distance(m_Children.begin(), it));

		int maxIndex = static_cast<int>(m_Children.size() - 1);
		int targetIndex = std::min(insertIndex, maxIndex);

		if (targetIndex == currentIndex)
			return;

		std::unique_ptr<SceneObject> owned = std::move(*it);
		m_Children.erase(it);

		m_Children.insert(m_Children.begin() + std::clamp(targetIndex, 0, static_cast<int>(m_Children.size())), std::move(owned));
	}

	std::unique_ptr<SceneObject> SceneObject::DetachChild(SceneObject* child)
	{
		for (auto it = m_Children.begin(); it != m_Children.end(); ++it)
		{
			if (it->get() == child)
			{
				auto detached = std::move(*it);
				m_Children.erase(it);
				detached->m_Parent = nullptr;
				return detached;
			}
		}
		
		return nullptr;
	}

	int SceneObject::GetChildIndex(const SceneObject* child) const
	{
		if (!child)
			return -1;

		auto it = std::find_if(m_Children.begin(), m_Children.end(),
			[child](const std::unique_ptr<SceneObject>& c)
			{
				return c.get() == child;
			});

		if (it == m_Children.end())
			return -1;

		return static_cast<int>(std::distance(m_Children.begin(), it));
	}

	const std::vector<std::unique_ptr<SceneObject>>& SceneObject::GetChildren() const
	{
		return m_Children;
	}

	std::vector<std::unique_ptr<SceneObject>>& SceneObject::GetChildren()
	{
		return m_Children;
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
