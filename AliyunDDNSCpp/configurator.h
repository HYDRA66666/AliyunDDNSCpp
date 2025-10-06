#pragma once

#include "framework.h"
#include "pch.h"

#include <nlohmann/json.hpp>

namespace HYDRA15::AliyunDDNSCpp
{
	class configurator
	{
		std::string accessKeyID;
		std::string accessKeySecret;
		std::string lastIPv4;
		std::string lastIPv6;

		nlohmann::json configJson;
	public:
		configurator();
		~configurator();

		static void config_accesskey(configurator cfg);
	}
}