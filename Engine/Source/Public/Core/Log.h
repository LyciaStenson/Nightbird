#pragma once

#include <string>

namespace Nightbird::Core
{
	class Log
	{
	public:
		using LogFunc = void(*)(const std::string&);

		static void Info(const std::string& message);
		static void Warning(const std::string& message);
		static void Error(const std::string& message);

		static void SetInfoFunc(LogFunc func);
		static void SetWarningFunc(LogFunc func);
		static void SetErrorFunc(LogFunc func);

	private:
		static LogFunc s_InfoFunc;
		static LogFunc s_WarningFunc;
		static LogFunc s_ErrorFunc;
	};
}
