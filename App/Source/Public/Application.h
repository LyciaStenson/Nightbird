#pragma once

#include "Core/Engine.h"
#include "Core/Platform.h"
#include "Core/Renderer.h"
#include "Core/BinaryAssetManager.h"

#include <memory>

namespace Nightbird::App
{
	class Application
	{
	public:
		Application() = default;
		~Application() = default;

		int Run();

	private:
		void Initialize();
		int LoadProject();
		void RunLoop();
		void Shutdown();
		
		std::unique_ptr<Core::Platform> m_Platform;
		std::unique_ptr<Core::Renderer> m_Renderer;
		std::unique_ptr<Core::BinaryAssetManager> m_AssetManager;
		std::unique_ptr<Core::Engine> m_Engine;
	};
}