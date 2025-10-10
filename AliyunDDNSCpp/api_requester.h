#pragma once
#include "ddns_framework.h"
#include "pch.h"

#include "config.h"
#include "initializer.h"


using namespace HYDRA15::Union;

namespace HYDRA15::AliyunDDNSCpp
{
	// 处理 api 请求
	// 主要负责：计算签名，发送请求
	class api_requester_sdkv2
	{
		// 请求参数
	private:
		const std::string accessKeyID;
		const std::string accessKeySecret;
	protected:
		std::unordered_map<std::string, std::string> params;

		std::string fullUrl;

		// 辅助函数
	private:
		static std::string percent_encode(const std::string& url);
		static std::string base64_encode(const std::vector<unsigned char>& str);
		static std::string hmac_sha1_base64(const std::string& key, const std::string& data);
		static std::string generate_noce();

		void calculate_signature();
		// 方法
	protected:
		api_requester_sdkv2();
		api_requester_sdkv2(std::unordered_map<std::string, std::string> ps);
		httplib::Result send_request();

	public:
		virtual ~api_requester_sdkv2() = default;

	};
}