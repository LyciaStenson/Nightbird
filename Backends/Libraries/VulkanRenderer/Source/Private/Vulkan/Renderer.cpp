#include "Vulkan/Renderer.h"

#include "Core/Platform.h"
#include "Core/Scene.h"
#include "Core/Camera.h"
#include "Core/RenderSurface.h"
#include "Core/TypeInfo.h"
#include "Core/Log.h"

#include "Vulkan/CameraUBO.h"
#include "Vulkan/LightData.h"
#include "Vulkan/Config.h"

#include <algorithm>

#include <glm/gtc/type_ptr.hpp>

namespace Nightbird::Vulkan
{
	Renderer::Renderer(Core::Platform* platform, std::vector<const char*> extensions, SurfaceCreator surfaceCreator)
		: m_Platform(platform), m_Extensions(std::move(extensions)), m_SurfaceCreator(surfaceCreator)
	{

	}

	void Renderer::Initialize()
	{
		m_Instance = std::make_unique<Instance>(m_Extensions);
		m_Surface = m_SurfaceCreator(m_Instance->Get());

		m_Device = std::make_unique<Device>(m_Instance->Get(), m_Surface);
		m_Sync = std::make_unique<Sync>(m_Device->GetLogical());

		m_DefaultTexture = CreateDefaultTexture();
		m_DefaultCubemap = std::make_shared<Texture>(Texture::CreateDefaultCubemap(m_Device.get()));
		CreateSkyboxGeometry();

		int width = 0;
		int height = 0;
		m_Platform->GetFramebufferSize(&width, &height);

		m_SwapChain = std::make_unique<SwapChain>(m_Device.get(), m_Sync.get(), m_Surface, static_cast<uint32_t>(width), static_cast<uint32_t>(height));

		m_SwapChainSurface = std::make_unique<SwapChainSurface>(m_Platform, *m_Device, *m_Sync, *m_SwapChain);

		CreateDescriptorPool();

		m_DescriptorSetLayoutManager = std::make_unique<DescriptorSetLayoutManager>(m_Device.get());
		m_FrameDescriptorSetManager = std::make_unique<FrameDescriptorSetManager>(m_Device.get(), m_DescriptorSetLayoutManager->GetFrameDescriptorSetLayout(), m_DescriptorPool);
		m_EnvironmentDescriptorSetManager = std::make_unique<EnvironmentDescriptorSetManager>(m_Device.get(), m_DescriptorSetLayoutManager->GetEnvironmentDescriptorSetLayout(), m_DescriptorPool);

		for (uint32_t i = 0; i < Config::MAX_FRAMES_IN_FLIGHT; ++i)
			m_EnvironmentDescriptorSetManager->UpdateSkybox(i, m_DefaultCubemap->GetImageView(), m_DefaultCubemap->GetSampler());

		m_TransformPool = std::make_unique<TransformPool>(m_Device.get(), m_DescriptorPool, m_DescriptorSetLayoutManager.get(), 1000);

		Core::Log::Info("Vulkan Renderer Initialized");
	}

	void Renderer::InitializeSurface(Core::RenderSurface& coreSurface)
	{
		RenderSurface& surface = static_cast<RenderSurface&>(coreSurface);
		RenderPass& renderPass = surface.GetRenderPass();

		PipelineConfig opaqueConfig;
		opaqueConfig.vertexShaderName = "Pbr.vert.spv";
		opaqueConfig.fragShaderName = "Pbr.frag.spv";
		opaqueConfig.descriptorSetLayouts = {
			m_DescriptorSetLayoutManager->GetFrameDescriptorSetLayout(),
			m_DescriptorSetLayoutManager->GetMeshDescriptorSetLayout(),
			m_DescriptorSetLayoutManager->GetMaterialDescriptorSetLayout()
		};
		opaqueConfig.depthTestEnable = true;
		opaqueConfig.depthWriteEnable = true;
		opaqueConfig.depthCompareOp = VK_COMPARE_OP_LESS;
		opaqueConfig.cullMode = VK_CULL_MODE_BACK_BIT;
		opaqueConfig.blendEnable = false;
		opaqueConfig.vertexLayout = VertexLayout::CreatePbrVertexLayout();
		m_OpaquePipeline = std::make_unique<Pipeline>(m_Device.get(), &renderPass, opaqueConfig);

		PipelineConfig transparentConfig = opaqueConfig;
		transparentConfig.depthWriteEnable = false;
		transparentConfig.blendEnable = true;
		m_TransparentPipeline = std::make_unique<Pipeline>(m_Device.get(), &renderPass, transparentConfig);

		PipelineConfig skyboxConfig;
		skyboxConfig.vertexShaderName = "Skybox.vert.spv";
		skyboxConfig.fragShaderName = "Skybox.frag.spv";
		skyboxConfig.descriptorSetLayouts = {
			m_DescriptorSetLayoutManager->GetFrameDescriptorSetLayout(),
			m_DescriptorSetLayoutManager->GetEnvironmentDescriptorSetLayout()
		};
		skyboxConfig.depthTestEnable = true;
		skyboxConfig.depthWriteEnable = false;
		skyboxConfig.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		skyboxConfig.cullMode = VK_CULL_MODE_FRONT_BIT;
		skyboxConfig.blendEnable = false;
		skyboxConfig.vertexLayout = VertexLayout::CreatePosVertexLayout();
		m_SkyboxPipeline = std::make_unique<Pipeline>(m_Device.get(), &renderPass, skyboxConfig);
	}

	void Renderer::Shutdown()
	{
		vkDeviceWaitIdle(m_Device->GetLogical());

		m_GeometryCache.clear();
		m_MaterialCache.clear();
		m_TextureCache.clear();
		m_CubemapCache.clear();

		m_TransformPool.reset();

		m_OpaquePipeline.reset();
		m_TransparentPipeline.reset();
		m_SkyboxPipeline.reset();

		m_FrameDescriptorSetManager.reset();
		m_EnvironmentDescriptorSetManager.reset();
		m_DescriptorSetLayoutManager.reset();

		vkDestroyDescriptorPool(m_Device->GetLogical(), m_DescriptorPool, nullptr);

		m_Sync.reset();
		m_SwapChainSurface.reset();
		m_SwapChain.reset();

		vkDestroySurfaceKHR(m_Instance->Get(), m_Surface, nullptr);

		m_DefaultTexture.reset();
		m_DefaultCubemap.reset();
		m_SkyboxGeometry.reset();

		m_Device.reset();
		m_Instance.reset();
	}

	void Renderer::SubmitScene(const Core::Scene& scene, const Core::Camera& camera)
	{
		m_ActiveCamera = &camera;
		m_Renderables = scene.CollectRenderables();
		m_DirectionalLights = scene.CollectDirectionalLights();
		m_PointLights = scene.CollectPointLights();
		m_Skybox = scene.FindSkybox();
	}

	bool Renderer::BeginFrame(Core::RenderSurface& coreSurface)
	{
		Vulkan::RenderSurface& surface = static_cast<Vulkan::RenderSurface&>(coreSurface);

		if (surface.GetSurfaceType() == RenderSurfaceType::SwapChain)
		{
			SwapChainSurface& swapChainSurface = static_cast<SwapChainSurface&>(surface);
			if (swapChainSurface.NeedsResize())
			{
				vkDeviceWaitIdle(m_Device->GetLogical());

				int width = 0;
				int height = 0;

				m_Platform->GetFramebufferSize(&width, &height);

				swapChainSurface.Resize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
				return false;
			}

			vkWaitForFences(m_Device->GetLogical(), 1, &m_Sync->m_InFlightFences[m_CurrentFrame.frameIndex], VK_TRUE, UINT64_MAX);

			VkFramebuffer framebuffer = swapChainSurface.AcquireFramebuffer(m_Sync->m_ImageAvailableSemaphores[m_CurrentFrame.frameIndex]);
			if (framebuffer == VK_NULL_HANDLE)
				return false;

			vkResetFences(m_Device->GetLogical(), 1, &m_Sync->m_InFlightFences[m_CurrentFrame.frameIndex]);

			m_CurrentFrame.commandBuffer = m_Device->GetCommandBuffer(m_CurrentFrame.frameIndex);
			vkResetCommandBuffer(m_CurrentFrame.commandBuffer, 0);

			RenderPass& renderPass = swapChainSurface.GetRenderPass();
			renderPass.BeginCommandBuffer(m_CurrentFrame.commandBuffer);
			renderPass.Begin(m_CurrentFrame.commandBuffer, framebuffer, swapChainSurface.GetExtent());

			return true;
		}
		else if (surface.GetSurfaceType() == RenderSurfaceType::Offscreen)
		{
			OffscreenSurface& offscreenSurface = static_cast<OffscreenSurface&>(surface);
			offscreenSurface.m_CommandBuffer = m_Device->BeginSingleTimeCommands();
			offscreenSurface.GetColorTexture().TransitionToColor(offscreenSurface.m_CommandBuffer);

			RenderPass& renderPass = offscreenSurface.GetRenderPass();
			renderPass.Begin(offscreenSurface.m_CommandBuffer, offscreenSurface.GetFramebuffer(), offscreenSurface.GetExtent());

			return true;
		}

		return false;
	}

	void Renderer::EndFrame(Core::RenderSurface& coreSurface)
	{
		Vulkan::RenderSurface& surface = static_cast<Vulkan::RenderSurface&>(coreSurface);

		if (surface.GetSurfaceType() == RenderSurfaceType::SwapChain)
		{
			SwapChainSurface& swapChainSurface = static_cast<SwapChainSurface&>(surface);

			RenderPass& renderPass = swapChainSurface.GetRenderPass();
			renderPass.End(m_CurrentFrame.commandBuffer);
			renderPass.EndCommandBuffer(m_CurrentFrame.commandBuffer);

			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

			VkSemaphore waitSemaphores[] = { m_Sync->m_ImageAvailableSemaphores[m_CurrentFrame.frameIndex] };
			VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = waitSemaphores;
			submitInfo.pWaitDstStageMask = waitStages;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &m_CurrentFrame.commandBuffer;

			VkSemaphore signalSemaphores[] = { m_Sync->m_RenderFinishedSemaphores[m_CurrentFrame.frameIndex] };
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = signalSemaphores;

			if (vkQueueSubmit(m_Device->GetGraphicsQueue(), 1, &submitInfo, m_Sync->m_InFlightFences[m_CurrentFrame.frameIndex]) != VK_SUCCESS)
			{
				Core::Log::Error("Failed to submit draw command buffer");
				return;
			}

			swapChainSurface.Present(m_Sync->m_RenderFinishedSemaphores[m_CurrentFrame.frameIndex]);

			m_CurrentFrame.frameIndex = (m_CurrentFrame.frameIndex + 1) % Config::MAX_FRAMES_IN_FLIGHT;
		}
		else if (surface.GetSurfaceType() == RenderSurfaceType::Offscreen)
		{
			OffscreenSurface& offscreenSurface = static_cast<OffscreenSurface&>(surface);

			RenderPass& renderPass = offscreenSurface.GetRenderPass();
			renderPass.End(offscreenSurface.m_CommandBuffer);

			offscreenSurface.GetColorTexture().SetImageLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

			m_Device->EndSingleTimeCommands(offscreenSurface.m_CommandBuffer);
			offscreenSurface.m_CommandBuffer = VK_NULL_HANDLE;
		}
	}

	void Renderer::DrawScene(Core::RenderSurface& coreSurface)
	{
		Vulkan::RenderSurface& surface = static_cast<Vulkan::RenderSurface&>(coreSurface);

		if (surface.GetSurfaceType() == RenderSurfaceType::SwapChain)
		{
			SwapChainSurface& swapChainSurface = static_cast<SwapChainSurface&>(surface);
			DrawScene(m_CurrentFrame.commandBuffer, swapChainSurface.GetExtent(), m_CurrentFrame.frameIndex);
		}
		else if (surface.GetSurfaceType() == RenderSurfaceType::Offscreen)
		{
			OffscreenSurface& offscreenSurface = static_cast<OffscreenSurface&>(surface);
			DrawScene(offscreenSurface.m_CommandBuffer, offscreenSurface.GetExtent(), 0);
		}
	}

	void Renderer::DrawScene(VkCommandBuffer commandBuffer, VkExtent2D extent, uint32_t frameIndex)
	{
		if (!m_ActiveCamera)
			return;

		m_TransformPool->Reset();

		CameraUBO cameraUBO{};
		cameraUBO.view = m_ActiveCamera->GetViewMatrix();
		glm::mat4 proj = m_ActiveCamera->GetProjectionMatrix(static_cast<float>(extent.width), static_cast<float>(extent.height));
		proj[1][1] *= -1;
		cameraUBO.projection = proj;
		cameraUBO.position = glm::vec4(m_ActiveCamera->GetWorldMatrix()[3]);

		m_FrameDescriptorSetManager->UpdateCamera(frameIndex, cameraUBO);

		std::vector<DirectionalLightData> directionalLightData;
		for (const auto* directionalLight : m_DirectionalLights)
		{
			DirectionalLightData data{};
			glm::mat4 worldMatrix = directionalLight->GetWorldMatrix();
			glm::vec3 forward = glm::normalize(glm::vec3(worldMatrix * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)));
			data.direction = glm::vec4(forward, 0.0f);
			glm::vec3 color = directionalLight->m_Color;
			data.colorIntensity = glm::vec4(color, directionalLight->m_Intensity);
			directionalLightData.push_back(data);
		}
		m_FrameDescriptorSetManager->UpdateDirectionalLights(frameIndex, directionalLightData);

		std::vector<PointLightData> pointLightData;
		for (const auto* pointLight : m_PointLights)
		{
			PointLightData data{};
			glm::vec3 worldPos = glm::vec3(pointLight->GetWorldMatrix()[3]);
			data.positionRadius = glm::vec4(worldPos, pointLight->m_Radius);
			glm::vec3 color = pointLight->m_Color;
			data.colorIntensity = glm::vec4(color, pointLight->m_Intensity);
			pointLightData.push_back(data);
		}
		m_FrameDescriptorSetManager->UpdatePointLights(frameIndex, pointLightData);

		if (m_Skybox)
		{
			auto cubemap = m_Skybox->m_Cubemap.Get();
			if (cubemap)
			{
				Texture& texture = GetOrCreateCubemap(cubemap.get());
				m_EnvironmentDescriptorSetManager->UpdateSkybox(frameIndex, texture.GetImageView(), texture.GetSampler());
			}
		}
		else
		{
			m_EnvironmentDescriptorSetManager->UpdateSkybox(frameIndex, m_DefaultCubemap->GetImageView(), m_DefaultCubemap->GetSampler());
		}

		std::vector<const Core::Renderable*> opaqueRenderables;
		std::vector<const Core::Renderable*> transparentRenderables;

		for (const auto& renderable : m_Renderables)
		{
			if (renderable.primitive->GetMaterial()->transparencyEnabled)
				transparentRenderables.push_back(&renderable);
			else
				opaqueRenderables.push_back(&renderable);
		}

		glm::vec3 cameraPos = glm::vec3(m_ActiveCamera->GetWorldMatrix()[3]);
		std::sort(transparentRenderables.begin(), transparentRenderables.end(),
			[&](const Core::Renderable* a, const Core::Renderable* b)
			{
				float distA = glm::length(cameraPos - glm::vec3(a->transform[3]));
				float distB = glm::length(cameraPos - glm::vec3(b->transform[3]));
				return distA > distB;
			}
		);

		m_OpaquePipeline->Bind(commandBuffer);
		for (const auto* renderable : opaqueRenderables)
			DrawRenderable(commandBuffer, *renderable, m_OpaquePipeline.get(), frameIndex);

		m_TransparentPipeline->Bind(commandBuffer);
		for (const auto* renderable : transparentRenderables)
			DrawRenderable(commandBuffer, *renderable, m_TransparentPipeline.get(), frameIndex);

		m_SkyboxPipeline->Bind(commandBuffer);
		DrawSkybox(commandBuffer, frameIndex);
	}

	void Renderer::DrawRenderable(VkCommandBuffer commandBuffer, const Core::Renderable& renderable, Pipeline* currentPipeline, uint32_t frameIndex)
	{
		Geometry& geometry = GetOrCreateGeometry(renderable.primitive);
		Material& material = GetOrCreateMaterial(renderable.primitive->GetMaterial().get());

		VkDescriptorSet transformDescriptorSet = m_TransformPool->Acquire(renderable.transform, frameIndex);

		VkBuffer vertexBuffers[] = { geometry.GetVertexBuffer() };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer, geometry.GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT16);

		std::array<VkDescriptorSet, 3> descriptorSets = {
			m_FrameDescriptorSetManager->GetDescriptorSets()[frameIndex], transformDescriptorSet, material.GetDescriptorSets()[frameIndex]
		};
		
		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, currentPipeline->GetLayout(), 0, static_cast<uint32_t>(descriptorSets.size()), descriptorSets.data(), 0, nullptr);
		vkCmdDrawIndexed(commandBuffer, geometry.GetIndexCount(), 1, 0, 0, 0);
	}

	void Renderer::DrawSkybox(VkCommandBuffer commandBuffer, uint32_t frameIndex)
	{
		if (!m_Skybox)
			return;

		VkBuffer vertexBuffers[] = { m_SkyboxGeometry->GetVertexBuffer() };
		VkDeviceSize offsets[] = { 0 };

		vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
		vkCmdBindIndexBuffer(commandBuffer, m_SkyboxGeometry->GetIndexBuffer(), 0, VK_INDEX_TYPE_UINT16);

		std::array<VkDescriptorSet, 2> descriptorSets = {
			m_FrameDescriptorSetManager->GetDescriptorSets()[frameIndex],
			m_EnvironmentDescriptorSetManager->GetDescriptorSets()[frameIndex]
		};

		vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_SkyboxPipeline->GetLayout(), 0, static_cast<uint32_t>(descriptorSets.size()), descriptorSets.data(), 0, nullptr);
		vkCmdDrawIndexed(commandBuffer, m_SkyboxGeometry->GetIndexCount(), 1, 0, 0, 0);
	}

	void Renderer::CreateDescriptorPool()
	{
		std::array<VkDescriptorPoolSize, 2> poolSizes{};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = 10000;

		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = 10000;

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = 10000;

		if (vkCreateDescriptorPool(m_Device->GetLogical(), &poolInfo, nullptr, &m_DescriptorPool) != VK_SUCCESS)
		{
			Core::Log::Error("Failed to create descriptor pool");
		}
	}

	std::shared_ptr<Core::Texture> Renderer::CreateDefaultTexture()
	{
		std::vector<uint8_t> pixels = {255, 255, 255, 255};
		return std::make_shared<Core::Texture>(1, 1, Core::TextureFormat::RGBA8, pixels);
	}

	void Renderer::CreateSkyboxGeometry()
	{
		std::vector<Core::VertexPos> vertices = {
			// Back face
			{{-1.0f, -1.0f, -1.0f}},
			{{ 1.0f, -1.0f, -1.0f}},
			{{ 1.0f,  1.0f, -1.0f}},
			{{-1.0f,  1.0f, -1.0f}},

			// Front face
			{{ 1.0f, -1.0f,  1.0f}},
			{{-1.0f, -1.0f,  1.0f}},
			{{-1.0f,  1.0f,  1.0f}},
			{{ 1.0f,  1.0f,  1.0f}},
		};
		
		std::vector<uint16_t> indices = {
			// Back
			2, 1, 0, 0, 3, 2,

			// Front
			6, 5, 4, 4, 7, 6,

			// Left
			3, 0, 5, 5, 6, 3,

			// Right
			7, 4, 1, 1, 2, 7,

			// Bottom
			1, 4, 5, 5, 0, 1,

			// Top
			7, 2, 3, 3, 6, 7
		};

		m_SkyboxGeometry = std::make_unique<Geometry>(m_Device.get(), vertices.data(), sizeof(Core::VertexPos) * vertices.size(), indices.data(), sizeof(uint16_t) * indices.size(), static_cast<uint32_t>(indices.size()));
	}

	Geometry& Renderer::GetOrCreateGeometry(const Core::MeshPrimitive* primitive)
	{
		auto it = m_GeometryCache.find(primitive);
		if (it != m_GeometryCache.end())
			return it->second;

		const auto vertices = primitive->GetVertices();
		const auto indices = primitive->GetIndices();
		m_GeometryCache.emplace(primitive, Geometry(m_Device.get(), vertices.data(), sizeof(vertices[0]) * vertices.size(), indices.data(), sizeof(indices[0]) * indices.size(), static_cast<uint32_t>(indices.size())));
		return m_GeometryCache.at(primitive);
	}

	Material& Renderer::GetOrCreateMaterial(const Core::Material* material)
	{
		auto it = m_MaterialCache.find(material);
		if (it != m_MaterialCache.end())
			return it->second;

		m_MaterialCache.emplace(material, Material(m_Device.get(), *material, m_DescriptorPool, m_DescriptorSetLayoutManager.get(), *m_DefaultTexture));
		return m_MaterialCache.at(material);
	}

	Texture& Renderer::GetOrCreateTexture(const Core::Texture* texture)
	{
		auto it = m_TextureCache.find(texture);
		if (it != m_TextureCache.end())
			return it->second;

		m_TextureCache.emplace(texture, Texture(m_Device.get(), *texture));
		return m_TextureCache.at(texture);
	}

	Texture& Renderer::GetOrCreateCubemap(const Core::Cubemap* cubemap)
	{
		auto it = m_CubemapCache.find(cubemap);
		if (it != m_CubemapCache.end())
			return it->second;

		if (!cubemap->HasData())
			Core::Log::Error("Vulkan::Renderer: Cubemap has no data for GPU upload");

		m_CubemapCache.emplace(cubemap, Texture(m_Device.get(), *cubemap));
		const_cast<Core::Cubemap*>(cubemap)->DiscardData();
		return m_CubemapCache.at(cubemap);
	}

	Core::RenderSurface& Renderer::GetDefaultSurface()
	{
		return *m_SwapChainSurface;
	}

	std::unique_ptr<Core::RenderSurface> Renderer::CreateOffscreenSurface(uint32_t width, uint32_t height, Core::RenderSurfaceFormat format)
	{
		VkFormat colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
		if (format == Core::RenderSurfaceFormat::DepthOnly)
			colorFormat = VK_FORMAT_UNDEFINED;

		VkFormat depthFormat = m_Device->FindSupportedDepthFormat();

		return std::make_unique<OffscreenSurface>(m_Device.get(), width, height, colorFormat, depthFormat);
	}

	Instance& Renderer::GetInstance()
	{
		return *m_Instance;
	}

	Device& Renderer::GetDevice()
	{
		return *m_Device;
	}

	SwapChain& Renderer::GetSwapChain()
	{
		return *m_SwapChain;
	}

	VkCommandBuffer Renderer::GetCurrentCommandBuffer() const
	{
		return m_CurrentFrame.commandBuffer;
	}
}
