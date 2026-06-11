#include "EditorBackendFactory.h"

#include "EditorUIBackend.h"
#include "GlfwImGuiPlatform.h"
#include "VulkanImGuiRenderer.h"
#include "Glfw/Platform.h"
#include "Vulkan/Renderer.h"
#include "Core/Platform.h"
#include "Core/Renderer.h"
#include "Core/Log.h"

namespace Nightbird::Editor
{
	std::unique_ptr<EditorUIBackend> CreateEditorUIBackend(Core::Platform& platform, Core::Renderer& renderer)
	{
		Core::Log::Info("CreateEditorUIRenderer");

		auto imGuiPlatform = std::make_unique<GlfwImGuiPlatform>(static_cast<Glfw::Platform&>(platform));
		auto imGuiRenderer = std::make_unique<VulkanImGuiRenderer>(static_cast<Vulkan::Renderer&>(renderer));

		return std::make_unique<EditorUIBackend>(std::move(imGuiPlatform), std::move(imGuiRenderer));
	}
}
