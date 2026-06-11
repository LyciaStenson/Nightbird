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
		WHBGfxInitShaderAttribute(&m_ShaderGroup, "aBaseColorTexCoord", 1, 0, GX2_ATTRIB_FORMAT_FLOAT_32_32);
		WHBGfxInitFetchShader(&m_ShaderGroup);

		for (uint32_t i = 0; i < m_ShaderGroup.vertexShader->uniformBlockCount; i++)
		{
			auto& b = m_ShaderGroup.vertexShader->uniformBlocks[i];
			if (std::string(b.name) == "CameraUBO")
				m_CameraBlockLocation = b.offset;
			else if (std::string(b.name) == "ModelUBO")
				m_ModelBlockLocation = b.offset;
		}
		
		// CameraUBO: view(16) + projection(16) + position(4) = 36 floats
		m_CameraData = (float*)MEMAllocFromDefaultHeapEx(36 * sizeof(float), GX2_UNIFORM_BLOCK_ALIGNMENT);

		// ModelUBO: model(16) = 16 floats
		//m_ModelData = (float*)MEMAllocFromDefaultHeapEx(16 * sizeof(float), GX2_UNIFORM_BLOCK_ALIGNMENT);

		std::vector<uint8_t> pixels = { 255, 255, 255, 255 };
		m_DefaultTexture = std::make_shared<Core::Texture>(1, 1, Core::TextureFormat::RGBA8, pixels);

		m_SurfaceTV = std::make_unique<RenderSurfaceTV>();
		m_SurfaceDRC = std::make_unique<RenderSurfaceDRC>();
	}

	void Renderer::Shutdown()
	{
		GX2DrawDone();

		MEMFreeToDefaultHeap(m_CameraData);
		//MEMFreeToDefaultHeap(m_ModelData);

		m_MaterialCache.clear();
		m_GeometryCache.clear();

		WHBGfxFreeShaderGroup(&m_ShaderGroup);
		WHBGfxShutdown();
	}

	void Renderer::SubmitScene(const Core::Scene& scene, const Core::Camera& camera)
	{
		m_ActiveCamera = &camera;
		m_Renderables = scene.CollectRenderables();
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

		glm::vec4 cameraPos = glm::vec4(glm::vec3(m_ActiveCamera->GetWorldMatrix()[3]), 1.0f);
		UploadMatrix(m_CameraData, m_ActiveCamera->GetViewMatrix());
		UploadMatrix(m_CameraData + 16, m_ActiveCamera->GetProjectionMatrix(static_cast<float>(surface.GetWidth()), static_cast<float>(surface.GetHeight())));
		UploadVec4(m_CameraData + 32, cameraPos);

		GX2Invalidate(GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_UNIFORM_BLOCK, m_CameraData, 36 * sizeof(float));
		GX2SetVertexUniformBlock(m_CameraBlockLocation, 36 * sizeof(float), m_CameraData);
		
		uint32_t renderableCount = static_cast<uint32_t>(m_Renderables.size());
		float* modelDataPool = (float*)MEMAllocFromDefaultHeapEx(16 * sizeof(float) * renderableCount, GX2_UNIFORM_BLOCK_ALIGNMENT);

		for (uint32_t i = 0; i < renderableCount; ++i)
		{
			float* modelData = modelDataPool + 16 * i;
			UploadMatrix(modelData, m_Renderables[i].transform);

			GX2Invalidate(GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_UNIFORM_BLOCK, modelData, 16 * sizeof(float));
			GX2SetVertexUniformBlock(m_ModelBlockLocation, 16 * sizeof(float), modelData);

			Geometry& geometry = GetOrCreateGeometry(m_Renderables[i].primitive);
			Material& material = GetOrCreateMaterial(m_Renderables[i].primitive->GetMaterial().get());

			GX2SetPixelTexture(&material.GetBaseColorTexture().GetTexture(), 3);
			GX2SetPixelSampler(&material.GetBaseColorTexture().GetSampler(), 3);

			GX2RSetAttributeBuffer(&geometry.GetPositionBuffer(), 0, geometry.GetPositionBuffer().elemSize, 0);
			GX2RSetAttributeBuffer(&geometry.GetTexCoordBuffer(), 1, geometry.GetTexCoordBuffer().elemSize, 0);
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
