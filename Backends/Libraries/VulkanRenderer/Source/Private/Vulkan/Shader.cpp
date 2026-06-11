#include "Vulkan/Shader.h"

#include "Core/Log.h"

#include <fstream>

namespace Nightbird::Vulkan
{
	Shader::Shader(VkDevice device, const std::string& filePath, VkShaderStageFlagBits stageFlag)
		: device(device)
	{
		std::vector<char> code = ReadFile(filePath);

		CreateShaderModule(code);

		stageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stageCreateInfo.stage = stageFlag;
		stageCreateInfo.module = shaderModule;
		stageCreateInfo.pName = "main";
	}

	Shader::~Shader()
	{
		if (shaderModule != VK_NULL_HANDLE)
			vkDestroyShaderModule(device, shaderModule, nullptr);
	}

	const VkPipelineShaderStageCreateInfo& Shader::GetStageCreateInfo() const
	{
		return stageCreateInfo;
	}

	void Shader::CreateShaderModule(const std::vector<char>& code)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
			Core::Log::Error("Failed to create shader module");
	}

	std::vector<char> Shader::ReadFile(const std::string& filePath)
	{
		std::ifstream file(filePath, std::ios::ate | std::ios::binary);
		if (!file.is_open())
		{
			Core::Log::Error("Failed to open file: " + filePath);
			return std::vector<char>();
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();

		return buffer;
	}
}