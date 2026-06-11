#pragma once

#include <memory>

namespace Nightbird::Core
{
	class Platform;
	class Renderer;
}

namespace Nightbird::Editor
{
	class EditorUIBackend;

	std::unique_ptr<EditorUIBackend> CreateEditorUIBackend(Nightbird::Core::Platform& platform, Nightbird::Core::Renderer& renderer);
}
