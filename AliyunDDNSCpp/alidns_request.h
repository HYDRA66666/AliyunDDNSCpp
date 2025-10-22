#pragma once
#include "ddns_framework.h"
#include "pch.h"

#include "config.h"
#include "api_requester.h"
#include "initializer.h"


using namespace HYDRA15::Union;

namespace HYDRA15::AliyunDDNSCpp::api_request
{
	// 发送具体的 api 请求
	// 在构造时自动设置请求参数。然后使用方法一键发送请求

	using api_requester = HYDRA15::AliyunDDNSCpp::api_requester_sdkv2;

	class record :public api_requester
	{
		const domain_info domainInfo;
		std::string type;
	public:
		record(domain_info di);
		record() = delete;
		virtual ~record() = default;

		std::pair<std::string,std::string> get();	// std::pair<RecordID, Value>
	};

	class addrecord :public api_requester
	{
		const domain_info domainInfo;
	public:
		addrecord(domain_info di); // 域名信息和值
		addrecord() = delete;
		virtual ~addrecord() = default;

		std::string post();
	};

	class update :public api_requester
	{
		const domain_info domainInfo;
	public:
		update(domain_info di); // 域名信息和值
		update() = delete;
		virtual ~update() = default;

		std::string post();
	};
}