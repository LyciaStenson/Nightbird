#pragma once

#include <memory>

#include "Input/InputSystem.h"
#include "Audio/AudioProvider.h"
#include "Core/AudioAsset.h"

namespace Nightbird::Core
{
	class Platform;
	class Renderer;
	class Scene;
	class AssetManager;

	class Engine
	{
	public:
		Engine(Platform& platform, Renderer& renderer, AssetManager& assetManager);
		~Engine();

		bool ShouldClose() const;
		float Update();

		Platform& GetPlatform();
		Renderer& GetRenderer();

		Input::System& GetInputSystem();
		Audio::Provider& GetAudioProvider();

		Scene& GetScene();
		void SetScene(std::unique_ptr<Scene> scene);

		AssetManager& GetAssetManager();

		float GetDeltaTime() const;

	private:
		Platform& m_Platform;
		Renderer& m_Renderer;

		AssetManager& m_AssetManager;

		std::unique_ptr<Scene> m_Scene;

		Input::System m_InputSystem;

		float m_DeltaTime = 0.0f;
	};
}
