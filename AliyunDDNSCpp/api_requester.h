#pragma once
#include "ddns_framework.h"
#include "pch.h"

#include "config.h"


using namespace HYDRA15::Union;

namespace HYDRA15::AliyunDDNSCpp
{
	// 处理 api 请求
	class api_requester_sdkv2
	{
		static_string aliApiVer = "2015-01-09";
		// 请求参数
	private:
		const std::string accessKeyID;
		const std::string accessKeySecret;
		nlohmann::json params;

		// 辅助函数
	private:

		// 方法
	public:
		api_requester_sdkv2();
		api_requester_sdkv2(nlohmann::json params);
		virtual ~api_requester_sdkv2();

		std::shared_ptr<httplib::Response> send_request();

	};
}