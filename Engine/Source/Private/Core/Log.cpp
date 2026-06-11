#include "Core/Log.h"

#include <iostream>

namespace Nightbird::Core
{
	static Log::LogFunc& GetInfoFunc()
	{
		static Log::LogFunc func = [](const std::string& message) { std::cout << "[Info] " << message << std::endl; };
		return func;
	}

	static Log::LogFunc& GetWarningFunc()
	{
		static Log::LogFunc func = [](const std::string& message) { std::cout << "[Warning] " << message << std::endl; };
		return func;
	}

	static Log::LogFunc& GetErrorFunc()
	{
		static Log::LogFunc func = [](const std::string& message) { std::cerr << "[Error] " << message << std::endl; };
		return func;
	}

	void Log::Info(const std::string& message)
	{
		GetInfoFunc()(message);
	}

	void Log::Warning(const std::string& message)
	{
		GetWarningFunc()(message);
	}

	void Log::Error(const std::string& message)
	{
		GetErrorFunc()(message);
	}

	void Log::SetInfoFunc(LogFunc func)
	{
		GetInfoFunc() = func;
	}

	void Log::SetWarningFunc(LogFunc func)
	{
		GetWarningFunc() = func;
	}

	void Log::SetErrorFunc(LogFunc func)
	{
		GetErrorFunc() = func;
	}
}
