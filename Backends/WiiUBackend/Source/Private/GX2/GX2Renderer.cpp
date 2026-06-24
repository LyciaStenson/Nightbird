#include "GX2/GX2Renderer.h"

#include "Core/Scene.h"
#include "Core/Camera.h"
#include "Core/MeshPrimitive.h"
#include "Core/Material.h"
#include "Core/Texture.h"
#include "Core/Log.h"

#include <coreinit/memdefaultheap.h>
#include <gfd.h>
#include <gx2/registers.h>
#include <gx2/draw.h>
#include <gx2/event.h>
#include <gx2/state.h>
#include <gx2/mem.h>
#include <gx2r/draw.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cstring>

#include "Shader_gsh.h"

namespace Nightbird::GX2
{
	static inline float swapF32(float f)
	{
		uint32_t i;
		memcpy(&i, &f, sizeof(i));
		i = __builtin_bswap32(i);
		memcpy(&f, &i, sizeof(f));
		return f;
	}

	static void UploadMatrix(float* dest, const glm::mat4& src)
	{
		const float* data = glm::value_ptr(src);
		for (int i = 0; i < 16; i++)
			dest[i] = swapF32(data[i]);
	}

	static void UploadVec4(float* dest, const glm::vec4& src)
	{
		dest[0] = swapF32(src.x);
		dest[1] = swapF32(src.y);
		dest[2] = swapF32(src.z);
		dest[3] = swapF32(src.w);
	}

	void Renderer::Initialize()
	{
		WHBGfxInit();

		WHBGfxLoadGFDShaderGroup(&m_ShaderGroup, 0, Shader_gsh);
		GX2Invalidate(GX2_INVALIDATE_MODE_CPU_SHADER, m_ShaderGroup.vertexShader->program, m_ShaderGroup.vertexShader->size);
		GX2Invalidate(GX2_INVALIDATE_MODE_CPU_SHADER, m_ShaderGroup.pixelShader->program, m_ShaderGroup.pixelShader->size);

		WHBGfxInitShaderAttribute(&m_ShaderGroup, "aPosition", 0, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32_32_32);
		WHBGfxInitShaderAttribute(&m_ShaderGroup, "aNormal", 1, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32_32);
		WHBGfxInitShaderAttribute(&m_ShaderGroup, "aBaseColorTexCoord", 2, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32);
		WHBGfxInitFetchShader(&m_ShaderGroup);

		// Find vertex uniform block locations
		for (uint32_t i = 0; i < m_ShaderGroup.vertexShader->uniformBlockCount; ++i)
		{
			auto& b = m_ShaderGroup.vertexShader->uniformBlocks[i];
			if (std::string(b.name) == "CameraUBO")
				m_CameraVertexBlockLoc = b.offset;
			else if (std::string(b.name) == "ModelUBO")
				m_ModelVertexBlockLoc = b.offset;
		}

		// Find pixel uniform block locations
		for (uint32_t i = 0; i < m_ShaderGroup.pixelShader->uniformBlockCount; ++i)
		{
			auto& b = m_ShaderGroup.pixelShader->uniformBlocks[i];
			if (std::string(b.name) == "CameraUBO")
				m_CameraPixelBlockLoc = b.offset;
			else if (std::string(b.name) == "DirectionalLightUBO")
				m_DirectionalLightPixelBlockLoc = b.offset;
			else if (std::string(b.name) == "PointLightUBO")
				m_PointLightPixelBlockLoc = b.offset;
			else if (std::string(b.name) == "AmbientLightUBO")
				m_AmbientLightPixelBlockLoc = b.offset;
		}

		Core::Log::Info("m_CameraVertexBlockLoc: " + std::to_string(m_CameraVertexBlockLoc));
		Core::Log::Info("m_ModelVertexBlockLoc: " + std::to_string(m_ModelVertexBlockLoc));
		Core::Log::Info("m_CameraPixelBlockLoc: " + std::to_string(m_CameraPixelBlockLoc));
		Core::Log::Info("m_DirectionalLightPixelBlockLoc: " + std::to_string(m_DirectionalLightPixelBlockLoc));
		Core::Log::Info("m_PointLightPixelBlockLoc: " + std::to_string(m_PointLightPixelBlockLoc));
		Core::Log::Info("m_AmbientLightPixelBlockLoc: " + std::to_string(m_AmbientLightPixelBlockLoc));
		
		// CameraUBO: view(16) + projection(16) + position(4) = 36 floats
		m_CameraData = (float*)MEMAllocFromDefaultHeapEx(36 * sizeof(float), GX2_UNIFORM_BLOCK_ALIGNMENT);
		
		// DirectionalLightUBO: 4 lights * (direction(4) + colorIntensity(4)) + count(1 padded to 4) = 36 floats
		m_DirectionalLightData = (float*)MEMAllocFromDefaultHeapEx(36 * sizeof(float), GX2_UNIFORM_BLOCK_ALIGNMENT);

		// PointLightUBO: 8 lights * (positionRadius(4) + colorIntensity(4)) + count(1 padded to 4) = 68 floats
		m_PointLightData = (float*)MEMAllocFromDefaultHeapEx(68 * sizeof(float), GX2_UNIFORM_BLOCK_ALIGNMENT);

		// AmbientLightUBO: colorIntensity(4) = 4 floats
		m_AmbientLightData = (float*)MEMAllocFromDefaultHeapEx(4 * sizeof(float), GX2_UNIFORM_BLOCK_ALIGNMENT);

		std::vector<uint8_t> pixels = { 255, 255, 255, 255 };
		m_DefaultTexture = std::make_shared<Core::Texture>(1, 1, Core::TextureFormat::RGBA8, pixels);

		m_SurfaceTV = std::make_unique<RenderSurfaceTV>();
		m_SurfaceDRC = std::make_unique<RenderSurfaceDRC>();
	}

	void Renderer::InitializeSurface(Core::RenderSurface& coreSurface)
	{

	}

	void Renderer::Shutdown()
	{
		GX2DrawDone();
		
		MEMFreeToDefaultHeap(m_CameraData);
		MEMFreeToDefaultHeap(m_DirectionalLightData);
		MEMFreeToDefaultHeap(m_PointLightData);
		MEMFreeToDefaultHeap(m_AmbientLightData);
		
		m_MaterialCache.clear();
		m_GeometryCache.clear();

		WHBGfxFreeShaderGroup(&m_ShaderGroup);
		WHBGfxShutdown();
	}

	void Renderer::SubmitScene(const Core::Scene& scene, const Core::Camera& camera)
	{
		m_ActiveCamera = &camera;
		m_Renderables = scene.CollectRenderables();
		m_DirectionalLights = scene.CollectDirectionalLights();
		m_PointLights = scene.CollectPointLights();
		m_AmbientLight = scene.FindAmbientLight();
	}

	bool Renderer::BeginFrame(Core::RenderSurface& surface)
	{
		WHBGfxBeginRender();
		return true;
	}

	void Renderer::EndFrame(Core::RenderSurface& surface)
	{
		WHBGfxFinishRender();
	}

	void Renderer::DrawScene(Core::RenderSurface& coreSurface)
	{
		if (!m_ActiveCamera)
			return;

		RenderSurface& surface = static_cast<RenderSurface&>(coreSurface);
		surface.Begin();
		
		WHBGfxClearColor(0.0f, 0.0f, 0.0f, 1.0f);

		GX2SetShaderMode(GX2_SHADER_MODE_UNIFORM_BLOCK);
		GX2SetFetchShader(&m_ShaderGroup.fetchShader);
		GX2SetVertexShader(m_ShaderGroup.vertexShader);
		GX2SetPixelShader(m_ShaderGroup.pixelShader);
		
		GX2SetViewport(0.0f, 0.0f, static_cast<float>(surface.GetWidth()), static_cast<float>(surface.GetHeight()), 0.0f, 1.0f);
		GX2SetScissor(0, 0, surface.GetWidth(), surface.GetHeight());

		GX2SetDepthOnlyControl(GX2_ENABLE, GX2_ENABLE, GX2_COMPARE_FUNC_LESS);
		GX2SetColorControl(GX2_LOGIC_OP_COPY, 0, GX2_DISABLE, GX2_ENABLE);
		GX2SetTargetChannelMasks(
			GX2_CHANNEL_MASK_RGBA,
			(GX2ChannelMask)0, (GX2ChannelMask)0, (GX2ChannelMask)0,
			(GX2ChannelMask)0, (GX2ChannelMask)0, (GX2ChannelMask)0,
			(GX2ChannelMask)0
		);

		// Upload camera to vertex shader
		glm::vec4 cameraPos = glm::vec4(glm::vec3(m_ActiveCamera->GetWorldMatrix()[3]), 1.0f);
		UploadMatrix(m_CameraData, m_ActiveCamera->GetViewMatrix());
		UploadMatrix(m_CameraData + 16, m_ActiveCamera->GetProjectionMatrix(static_cast<float>(surface.GetWidth()), static_cast<float>(surface.GetHeight())));
		UploadVec4(m_CameraData + 32, cameraPos);
		GX2Invalidate(GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_UNIFORM_BLOCK, m_CameraData, 36 * sizeof(float));
		GX2SetVertexUniformBlock(m_CameraVertexBlockLoc, 36 * sizeof(float), m_CameraData);
		
		// Upload camera to pixel shader
		GX2Invalidate(GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_UNIFORM_BLOCK, m_CameraData, 36 * sizeof(float));
		GX2SetPixelUniformBlock(m_CameraPixelBlockLoc, 36 * sizeof(float), m_CameraData);

		// Upload directional lights to pixel shader
		uint32_t directionalLightCount = static_cast<uint32_t>(std::min(m_DirectionalLights.size(), size_t(4)));
		float* directionalLightPtr = m_DirectionalLightData;
		for (uint32_t i = 0; i < directionalLightCount; ++i)
		{
			const Core::DirectionalLight* light = m_DirectionalLights[i];
			glm::mat4 worldMatrix = light->GetWorldMatrix();
			glm::vec3 forward = glm::normalize(glm::vec3(worldMatrix * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)));
			UploadVec4(directionalLightPtr + i * 8, glm::vec4(forward, 0.0f));
			UploadVec4(directionalLightPtr + i * 8 + 4, glm::vec4(light->m_Color, light->m_Intensity));
		}
		// Upload count padded to vec4
		uint32_t directionalLightCountSwapped = __builtin_bswap32(directionalLightCount);
		memcpy(m_DirectionalLightData + 32, &directionalLightCountSwapped, sizeof(uint32_t));
		GX2Invalidate(GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_UNIFORM_BLOCK, m_DirectionalLightData, 36 * sizeof(float));
		GX2SetPixelUniformBlock(m_DirectionalLightPixelBlockLoc, 36 * sizeof(float), m_DirectionalLightData);

		// Upload point lights to pixel shader
		uint32_t pointLightCount = static_cast<uint32_t>(std::min(m_PointLights.size(), size_t(8)));
		for (uint32_t i = 0; i < pointLightCount; ++i)
		{
			const Core::PointLight* light = m_PointLights[i];
			glm::vec3 worldPos = glm::vec3(light->GetWorldMatrix()[3]);
			UploadVec4(m_PointLightData + i * 8, glm::vec4(worldPos, light->m_Radius));
			UploadVec4(m_PointLightData + i * 8 + 4, glm::vec4(light->m_Color, light->m_Intensity));
		}
		// Upload count padded to vec4
		uint32_t pointCountSwapped = __builtin_bswap32(pointLightCount);
		memcpy(m_PointLightData + 64, &pointCountSwapped, sizeof(uint32_t));
		GX2Invalidate(GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_UNIFORM_BLOCK, m_PointLightData, 68 * sizeof(float));
		GX2SetPixelUniformBlock(m_PointLightPixelBlockLoc, 68 * sizeof(float), m_PointLightData);

		// Upload ambient light to pixel shader
		if (m_AmbientLight)
			UploadVec4(m_AmbientLightData, glm::vec4(m_AmbientLight->m_Color, m_AmbientLight->m_Intensity));
		else
			UploadVec4(m_AmbientLightData, glm::vec4(0.0f));
		GX2Invalidate(GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_UNIFORM_BLOCK, m_AmbientLightData, 4 * sizeof(float));
		GX2SetPixelUniformBlock(m_AmbientLightPixelBlockLoc, 4 * sizeof(float), m_AmbientLightData);
		
		uint32_t renderableCount = static_cast<uint32_t>(m_Renderables.size());

		static constexpr uint32_t MODEL_BLOCK_STRIDE = 64;

		float* modelDataPool = (float*)MEMAllocFromDefaultHeapEx(MODEL_BLOCK_STRIDE * sizeof(float) * renderableCount, GX2_UNIFORM_BLOCK_ALIGNMENT);

		for (uint32_t i = 0; i < renderableCount; ++i)
		{
			float* modelData = modelDataPool + MODEL_BLOCK_STRIDE * i;
			UploadMatrix(modelData, m_Renderables[i].transform);

			glm::mat4 normalMatrix = glm::transpose(glm::inverse(m_Renderables[i].transform));
			UploadMatrix(modelData + 16, normalMatrix);

			GX2Invalidate(GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_UNIFORM_BLOCK, modelData, MODEL_BLOCK_STRIDE * sizeof(float));
			GX2SetVertexUniformBlock(m_ModelVertexBlockLoc, MODEL_BLOCK_STRIDE * sizeof(float), modelData);

			Geometry& geometry = GetOrCreateGeometry(m_Renderables[i].primitive);
			Material& material = GetOrCreateMaterial(m_Renderables[i].primitive->GetMaterial().get());

			GX2SetPixelTexture(&material.GetBaseColorTexture().GetTexture(), 3);
			GX2SetPixelSampler(&material.GetBaseColorTexture().GetSampler(), 3);

			GX2RSetAttributeBuffer(&geometry.GetPositionBuffer(), 0, geometry.GetPositionBuffer().elemSize, 0);
			GX2RSetAttributeBuffer(&geometry.GetNormalBuffer(), 1, geometry.GetNormalBuffer().elemSize, 0);
			GX2RSetAttributeBuffer(&geometry.GetTexCoordBuffer(), 2, geometry.GetTexCoordBuffer().elemSize, 0);

			GX2DrawIndexedEx(GX2_PRIMITIVE_MODE_TRIANGLES, geometry.GetIndexCount(), GX2_INDEX_TYPE_U16, geometry.GetIndexBuffer().buffer, 0, 1);
		}
		
		MEMFreeToDefaultHeap(modelDataPool);
		surface.Finish();
	}

	Core::RenderSurface& Renderer::GetDefaultSurface()
	{
		return *m_SurfaceTV;
	}

	std::unique_ptr<Core::RenderSurface> Renderer::CreateOffscreenSurface(uint32_t width, uint32_t height, Core::RenderSurfaceFormat format)
	{
		return nullptr;
	}

	Geometry& Renderer::GetOrCreateGeometry(const Core::MeshPrimitive* primitive)
	{
		auto it = m_GeometryCache.find(primitive);
		if (it != m_GeometryCache.end())
			return it->second;

		// Create and add to cache if does not exist
		m_GeometryCache.emplace(primitive, Geometry(*primitive));
		return m_GeometryCache.at(primitive);
	}

	Material& Renderer::GetOrCreateMaterial(const Core::Material* material)
	{
		auto it = m_MaterialCache.find(material);
		if (it != m_MaterialCache.end())
			return it->second;

		// Create and add to cache if does not exist
		m_MaterialCache.emplace(material, Material(*material, *m_DefaultTexture));
		return m_MaterialCache.at(material);
	}
}
