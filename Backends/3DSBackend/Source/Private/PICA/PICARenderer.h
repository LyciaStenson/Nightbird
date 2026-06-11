#pragma once

#include "Core/Renderer.h"

#include "Core/Renderable.h"

#include "PICA/PICAGeometry.h"
#include "PICA/PICAMaterial.h"
#include "PICA/PICATexture.h"
#include "PICA/PICATopRenderSurface.h"

#include <citro3d.h>

#include <vector>
#include <unordered_map>
#include <memory>

namespace Nightbird::Core
{
	class Scene;
	class Camera;
	class MeshPrimitive;
	struct Material;
}

namespace Nightbird::PICA
{
	class Renderer : public Core::Renderer
	{
	public:
		void Initialize() override;
		void Shutdown() override;
		Core::RenderSurface& GetDefaultSurface() override;
		std::unique_ptr<Core::RenderSurface> CreateOffscreenSurface(uint32_t width, uint32_t height, Core::RenderSurfaceFormat format) override;
		void SubmitScene(const Core::Scene& scene, const Core::Camera& camera) override;
		bool BeginFrame(Core::RenderSurface& surface) override;
		void EndFrame(Core::RenderSurface& surface) override;
		void DrawScene(Core::RenderSurface& surface) override;
	
	private:
		const Core::Camera* m_ActiveCamera = nullptr;
		std::vector<Core::Renderable> m_Renderables;
		std::unordered_map<const Core::MeshPrimitive*, Geometry> m_GeometryCache;
		std::unordered_map<const Core::Material*, Material> m_MaterialCache;
		std::unordered_map<const Core::Texture*, std::shared_ptr<Texture>> m_TextureCache;

		std::shared_ptr<Texture> m_DefaultTexture;
		
		std::unique_ptr<TopRenderSurface> m_TopSurface;
		
		DVLB_s* m_ShaderDvlb = nullptr;
		shaderProgram_s m_ShaderProgram = {};
		int m_ULocProjection = -1;
		int m_ULocModelView = -1;

		Geometry& GetOrCreateGeometry(const Core::MeshPrimitive* primitive);
		Material& GetOrCreateMaterial(const Core::Material* material);
		std::shared_ptr<Texture> GetOrCreateTexture(const Core::Texture* texture);
	};
}
