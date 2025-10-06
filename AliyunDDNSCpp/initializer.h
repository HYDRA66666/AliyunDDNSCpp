#pragma once
#include "framework.h"
#include "pch.h"

#include "config.h"

using namespace HYDRA15::Union;

namespace HYDRA15::AliyunDDNSCpp 
{

	class initializer
	{
		// 私有变量
	private:
		std::fstream logFile;

		// 系统对象
	private:
		secretary::logger lgr{ "Initializer" };
		commander::Command& cmd = commander::Command::get_instance();
		secretary::PrintCenter& pc = secretary::PrintCenter::get_instance();

		// 配置项
	public:
		std::string accessKeyID;
		std::string accessKeySecret;
		std::string lastIPv4;
		std::string lastIPv6;

		// 标志位
	private:
		bool is_ready = true;
	public:
		bool ready() const;

		// 辅助函数
	private:
		static std::string get_registry_item(const std::string& item);
		static void set_registry_item(const std::string& item, const std::string& value);
		
	public:
		initializer();
		~initializer();
	};

	class hkey_guard
	{
		HKEY& hkey;
	public:
		hkey_guard(HKEY& k);
		~hkey_guard();
	};
}