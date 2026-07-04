#include "PICA/PICARenderer.h"

#include "Core/Camera.h"
#include "Core/MeshPrimitive.h"
#include "Core/Material.h"
#include "Core/Texture.h"
#include "Core/Log.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <3ds.h>

#include "Shader_v_pica_shbin.h"

namespace Nightbird::PICA
{
	void Renderer::Initialize()
	{
		C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
		
		m_TopSurface = std::make_unique<TopRenderSurface>();
		
		m_ShaderDvlb = DVLB_ParseFile((u32*)Shader_v_pica_shbin, Shader_v_pica_shbin_size);
		shaderProgramInit(&m_ShaderProgram);
		shaderProgramSetVsh(&m_ShaderProgram, &m_ShaderDvlb->DVLE[0]);
		C3D_BindProgram(&m_ShaderProgram);

		m_ULocProjection = shaderInstanceGetUniformLocation(m_ShaderProgram.vertexShader, "projection");
		m_ULocModelView = shaderInstanceGetUniformLocation(m_ShaderProgram.vertexShader, "modelView");

		C3D_AttrInfo* attrInfo = C3D_GetAttrInfo();
		AttrInfo_Init(attrInfo);
		AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 3); // v0 position
		AttrInfo_AddLoader(attrInfo, 1, GPU_FLOAT, 3); // v1 normal
		AttrInfo_AddLoader(attrInfo, 2, GPU_FLOAT, 2); // v2 texcoord

		C3D_TexEnv* env = C3D_GetTexEnv(0);
		C3D_TexEnvInit(env);
		C3D_TexEnvSrc(env, C3D_RGB, GPU_TEXTURE0, GPU_FRAGMENT_PRIMARY_COLOR, (GPU_TEVSRC)0);
		C3D_TexEnvFunc(env, C3D_RGB, GPU_MODULATE); // Multiply vertex and texture color

		C3D_TexEnv* env1 = C3D_GetTexEnv(1);
		C3D_TexEnvInit(env1);
		C3D_TexEnvSrc(env1, C3D_RGB, GPU_PREVIOUS, GPU_FRAGMENT_SECONDARY_COLOR, (GPU_TEVSRC)0);
		C3D_TexEnvFunc(env1, C3D_RGB, GPU_ADD);

		C3D_TexEnvSrc(env, C3D_Alpha, GPU_TEXTURE0, (GPU_TEVSRC)0, (GPU_TEVSRC)0);
		C3D_TexEnvFunc(env, C3D_Alpha, GPU_REPLACE);
		
		static const C3D_Material material =
		{
			{ 0.0f, 0.0f, 0.0f }, // Ambient
			{ 0.8f, 0.8f, 0.8f }, // Diffuse
			{ 0.5f, 0.5f, 0.5f }, // Specular0
			{ 0.0f, 0.0f, 0.0f }, // Specular1
			{ 0.0f, 0.0f, 0.0f }, // Emission
		};

		C3D_LightEnvInit(&m_LightEnv);
		C3D_LightEnvBind(&m_LightEnv);
		C3D_LightEnvMaterial(&m_LightEnv, &material);

		LightLut_Phong(&m_LutPhong, 30);
		C3D_LightEnvLut(&m_LightEnv, GPU_LUT_D0, GPU_LUTINPUT_LN, false, &m_LutPhong);
		
		uint8_t pixels[8*8*4]; // 8x8 RGBA
		for (int i = 0; i < 8*8*4; i++)
			pixels[i] = 255;

		m_DefaultTexture = std::make_shared<Texture>();
		m_DefaultTexture->InitFromPixels(8, 8, pixels);
	}

	void Renderer::InitializeSurface(Core::RenderSurface& coreSurface)
	{

	}

	void Renderer::Shutdown()
	{
		m_MaterialCache.clear();
		m_GeometryCache.clear();

		shaderProgramFree(&m_ShaderProgram);
		DVLB_Free(m_ShaderDvlb);

		m_TopSurface.reset();

		C3D_Fini();
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
		C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

		m_TopSurface->Begin();
		return true;
	}

	void Renderer::EndFrame(Core::RenderSurface& surface)
	{
		C3D_FrameEnd(0);
	}

	void Renderer::DrawScene(Core::RenderSurface& surface)
	{
		if (!m_ActiveCamera)
			return;

		C3D_LightEnvInit(&m_LightEnv);
		C3D_LightEnvBind(&m_LightEnv);

		C3D_Material material =
		{
			{ 0.0f, 0.0f, 0.0f }, // Ambient
			{ 0.8f, 0.8f, 0.8f }, // Diffuse
			{ 0.5f, 0.5f, 0.5f }, // Specular0
			{ 0.0f, 0.0f, 0.0f }, // Specular1
			{ 0.0f, 0.0f, 0.0f }, // Emission
		};
		if (m_AmbientLight)
		{
			material.ambient[0] = m_AmbientLight->m_Color.r * m_AmbientLight->m_Intensity;
			material.ambient[1] = m_AmbientLight->m_Color.g * m_AmbientLight->m_Intensity;
			material.ambient[2] = m_AmbientLight->m_Color.b * m_AmbientLight->m_Intensity;
		}
		C3D_LightEnvMaterial(&m_LightEnv, &material);

		LightLut_Phong(&m_LutPhong, 30);
		C3D_LightEnvLut(&m_LightEnv, GPU_LUT_D0, GPU_LUTINPUT_LN, false, &m_LutPhong);

		int lightIndex = 0;
		uint32_t directionalLightCount = static_cast<uint32_t>(std::min(m_DirectionalLights.size(), size_t(8)));
		for (uint32_t i = 0; i < directionalLightCount && lightIndex < 8; i++, lightIndex++)
		{
			const Core::DirectionalLight* light = m_DirectionalLights[i];
			glm::mat4 worldMatrix = light->GetWorldMatrix();
			glm::vec3 direction = glm::normalize(glm::vec3(worldMatrix * glm::vec4(0.0f, 0.0f, -1.0f, 0.0f)));

			C3D_FVec lightVec = FVec4_New(-direction.x, -direction.y, -direction.z, 0.0f);
			C3D_LightInit(&m_Lights[lightIndex], &m_LightEnv);
			C3D_LightColor(&m_Lights[lightIndex],
				light->m_Color.r * light->m_Intensity,
				light->m_Color.g * light->m_Intensity,
				light->m_Color.b * light->m_Intensity
			);
			C3D_LightPosition(&m_Lights[lightIndex], &lightVec);
		}

		uint32_t pointLightCount = static_cast<uint32_t>(std::min(m_PointLights.size(), size_t(8 - lightIndex)));
		for (uint32_t i = 0; i < pointLightCount && lightIndex < 8; i++, lightIndex++)
		{
			const Core::PointLight* light = m_PointLights[i];
			glm::vec3 worldPos = glm::vec3(light->GetWorldMatrix()[3]);

			C3D_FVec lightVec = FVec4_New(worldPos.x, worldPos.y, worldPos.z, 1.0f);
			C3D_LightInit(&m_Lights[lightIndex], &m_LightEnv);
			C3D_LightColor(&m_Lights[lightIndex],
				light->m_Color.r * light->m_Intensity,
				light->m_Color.g * light->m_Intensity,
				light->m_Color.b * light->m_Intensity
			);
			C3D_LightPosition(&m_Lights[lightIndex], &lightVec);
		}
		
		C3D_Mtx projection;
		Mtx_PerspTilt(&projection, C3D_AngleFromDegrees(m_ActiveCamera->m_Fov), C3D_AspectRatioTop, 0.01f, 1000.0f, false);

		C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, m_ULocProjection, &projection);

		for (const auto& renderable : m_Renderables)
		{
			Geometry& geometry = GetOrCreateGeometry(renderable.primitive);
			Material& material = GetOrCreateMaterial(renderable.primitive->GetMaterial().get());

			C3D_TexBind(0, material.GetBaseColorTexture()->GetTexture());
			//C3D_TexSetFilter(material.GetBaseColorTexture()->GetTexture(), GPU_LINEAR, GPU_NEAREST);

			C3D_Mtx modelView;
			Mtx_Identity(&modelView);

			glm::mat4 mv = m_ActiveCamera->GetViewMatrix() * renderable.transform;
			for (int row = 0; row < 4; row++)
			{
				modelView.r[row].c[0] = mv[3][row]; // w = col3
				modelView.r[row].c[1] = mv[2][row]; // z = col2
				modelView.r[row].c[2] = mv[1][row]; // y = col1
				modelView.r[row].c[3] = mv[0][row]; // x = col0
			}

			C3D_FVUnifMtx4x4(GPU_VERTEX_SHADER, m_ULocModelView, &modelView);

			C3D_BufInfo* bufInfo = C3D_GetBufInfo();
			BufInfo_Init(bufInfo);
			BufInfo_Add(bufInfo, geometry.GetVertexBuffer(), sizeof(PICA::Vertex), 3, 0x210);

			C3D_DrawElements(GPU_TRIANGLES, geometry.GetIndexCount(), C3D_UNSIGNED_SHORT, geometry.GetIndexBuffer());
		}
	}

	Core::RenderSurface& Renderer::GetDefaultSurface()
	{
		return *m_TopSurface;
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

		std::shared_ptr<PICA::Texture> tex;
		if (material->baseColorTexture)
			tex = GetOrCreateTexture(material->baseColorTexture.get());
		else
			tex = m_DefaultTexture;

		// Create and add to cache if does not exist
		m_MaterialCache.emplace(material, Material(*material, tex));
		return m_MaterialCache.at(material);
	}

	std::shared_ptr<Texture> Renderer::GetOrCreateTexture(const Core::Texture* texture)
	{
		auto it = m_TextureCache.find(texture);
		if (it != m_TextureCache.end())
			return it->second;

		// Create and add to cache if does not exist
		auto tex = std::make_shared<PICA::Texture>(*texture);
		m_TextureCache.emplace(texture, tex);
		return m_TextureCache.at(texture);
	}
}
