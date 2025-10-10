#pragma once
#include "ddns_framework.h"
#include "pch.h"

#include "config.h"
#include "initializer.h"

using namespace HYDRA15::Union;

namespace HYDRA15::AliyunDDNSCpp
{
	// 处理指令的函数
	class command_handler
	{
	public:
		static void getip(const std::list<std::string>&);
		static void get_ipv4(const std::list<std::string>&);
		static void get_ipv6(const std::list<std::string>&);
	};
}