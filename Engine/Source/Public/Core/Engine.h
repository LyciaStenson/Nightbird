#pragma once

#include "Core/Api.h"
#include "Core/Scene.h"
#include "Core/AudioAsset.h"
#include "Audio/AudioProvider.h"

#include "Input/InputSystem.h"

#include <memory>

namespace Nightbird::Core
{
	class Platform;
	class Renderer;
	class AssetManager;

	class NB_API Engine
	{
	public:
		Engine(Platform& platform, Renderer& renderer, AssetManager& assetManager);
		~Engine() = default;

		bool GetShouldClose() const;
		void Update(float deltaTime);

		Platform& GetPlatform();
		Renderer& GetRenderer();

		Input::System& GetInputSystem();
		Audio::Provider& GetAudioProvider();

		Scene& GetScene();
		void SetScene(std::unique_ptr<Scene> scene);

		AssetManager& GetAssetManager();

		float GetDeltaTime() const;

		bool m_Simulate = false;

	private:
		Platform& m_Platform;
		Renderer& m_Renderer;

		AssetManager& m_AssetManager;

		std::unique_ptr<Scene> m_Scene;

		Input::System m_InputSystem;

		float m_DeltaTime = 0.0f;
	};
}
