#pragma once

#include "Core/Renderer.h"

#include "Core/MeshPrimitive.h"
#include "Core/Material.h"
#include "Core/MeshInstance.h"
#include "Core/Texture.h"
#include "Core/Cubemap.h"
#include "Core/Renderable.h"
#include "Core/DirectionalLight.h"
#include "Core/PointLight.h"
#include "Core/Skybox.h"

#include "Vulkan/Instance.h"
#include "Vulkan/Device.h"
#include "Vulkan/SwapChain.h"
#include "Vulkan/Sync.h"
#include "Vulkan/DescriptorSetLayoutManager.h"
#include "Vulkan/EnvironmentDescriptorSetManager.h"
#include "Vulkan/FrameDescriptorSetManager.h"
#include "Vulkan/Pipeline.h"
#include "Vulkan/Geometry.h"
#include "Vulkan/Material.h"
#include "Vulkan/Texture.h"
#include "Vulkan/TransformPool.h"
#include "Vulkan/FrameContext.h"

#include "Vulkan/SwapChainSurface.h"
#include "Vulkan/OffscreenSurface.h"

#include <volk.h>

#include <vector>
#include <memory>
#include <unordered_map>

namespace Nightbird::Core
{
	class Platform;
	class Scene;
	class Camera;
}

namespace Nightbird::Vulkan
{
	using SurfaceCreator = VkSurfaceKHR(*)(VkInstance);
	
	class Renderer : public Core::Renderer
	{
	public:
		Renderer(Core::Platform* platform, std::vector<const char*> extensions, SurfaceCreator surfaceCreator);

		void Initialize() override;
		void InitializeSurface(Core::RenderSurface& surface) override;
		void Shutdown() override;
		Core::RenderSurface& GetDefaultSurface() override;
		std::unique_ptr<Core::RenderSurface> CreateOffscreenSurface(uint32_t width, uint32_t height, Core::RenderSurfaceFormat format) override;
		void SubmitScene(const Core::Scene& scene, const Core::Camera& camera) override;
		bool BeginFrame(Core::RenderSurface& surface) override;
		void EndFrame(Core::RenderSurface& surface) override;
		void DrawScene(Core::RenderSurface& surface) override;
		
		Instance& GetInstance();
		Device& GetDevice();
		SwapChain& GetSwapChain();
		
		VkCommandBuffer GetCurrentCommandBuffer() const;

	private:
		Core::Platform* m_Platform = nullptr;

		std::vector<const char*> m_Extensions;
		SurfaceCreator m_SurfaceCreator;
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

		std::unique_ptr<Instance> m_Instance;
		std::unique_ptr<Device> m_Device;
		std::unique_ptr<SwapChain> m_SwapChain;
		std::unique_ptr<SwapChainSurface> m_SwapChainSurface;
		std::unique_ptr<Sync> m_Sync;

		VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;

		std::unique_ptr<DescriptorSetLayoutManager> m_DescriptorSetLayoutManager;
		std::unique_ptr<FrameDescriptorSetManager> m_FrameDescriptorSetManager;
		std::unique_ptr<EnvironmentDescriptorSetManager> m_EnvironmentDescriptorSetManager;

		std::unique_ptr<Pipeline> m_OpaquePipeline;
		std::unique_ptr<Pipeline> m_TransparentPipeline;
		std::unique_ptr<Pipeline> m_SkyboxPipeline;

		std::unique_ptr<TransformPool> m_TransformPool;

		std::unordered_map<const Core::MeshPrimitive*, Geometry> m_GeometryCache;
		std::unordered_map<const Core::Material*, Material> m_MaterialCache;
		std::unordered_map<const Core::Texture*, Texture> m_TextureCache;
		std::unordered_map<const Core::Cubemap*, Texture> m_CubemapCache;

		const Core::Camera* m_ActiveCamera = nullptr;

		std::vector<Core::Renderable> m_Renderables;
		std::vector<Core::DirectionalLight*> m_DirectionalLights;
		std::vector<Core::PointLight*> m_PointLights;
		const Core::Skybox* m_Skybox = nullptr;
		std::unique_ptr<Geometry> m_SkyboxGeometry;

		FrameContext m_CurrentFrame;

		std::shared_ptr<Core::Texture> m_DefaultTexture;
		std::shared_ptr<Texture> m_DefaultCubemap;

		void DrawScene(VkCommandBuffer commandBuffer, VkExtent2D extent, uint32_t frameIndex);
		void DrawRenderable(VkCommandBuffer commandBuffer, const Core::Renderable&, Pipeline* currentPipeline, uint32_t frameIndex);
		void DrawSkybox(VkCommandBuffer commandBuffer, uint32_t frameIndex);

		void CreateDescriptorPool();

		std::shared_ptr<Core::Texture> CreateDefaultTexture();

		void CreateSkyboxGeometry();

		Geometry& GetOrCreateGeometry(const Core::MeshPrimitive* primitive);
		Material& GetOrCreateMaterial(const Core::Material* material);
		Texture& GetOrCreateTexture(const Core::Texture* texture);
		Texture& GetOrCreateCubemap(const Core::Cubemap* cubemap);
	};
}
