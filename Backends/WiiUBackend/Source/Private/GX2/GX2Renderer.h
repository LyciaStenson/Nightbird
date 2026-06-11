#pragma once

#include "Core/Renderer.h"

#include "Core/Renderable.h"

#include "GX2/GX2Geometry.h"
#include "GX2/GX2Material.h"
#include "GX2/GX2RenderSurfaceTV.h"
#include "GX2/GX2RenderSurfaceDRC.h"

#include <gx2/shaders.h>
#include <whb/gfx.h>

#include <vector>
#include <unordered_map>
#include <memory>

namespace Nightbird::Core
{
	class Scene;
	class Camera;
	class MeshPrimitive;
	class Texture;
	struct Material;
}

namespace Nightbird::GX2
{
	class RenderSurfaceTV;
	class RenderSurfaceDRC;

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

		std::shared_ptr<Core::Texture> m_DefaultTexture;

		std::unique_ptr<RenderSurfaceTV> m_SurfaceTV;
		std::unique_ptr<RenderSurfaceDRC> m_SurfaceDRC;
		
		WHBGfxShaderGroup m_ShaderGroup = {};

		uint32_t m_CameraBlockLocation = 0;
		uint32_t m_ModelBlockLocation = 0;

		float* m_CameraData = nullptr;
		//float* m_ModelData = nullptr;

		Geometry& GetOrCreateGeometry(const Core::MeshPrimitive* primitive);
		Material& GetOrCreateMaterial(const Core::Material* material);
	};
}
