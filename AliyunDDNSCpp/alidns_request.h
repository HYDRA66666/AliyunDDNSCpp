#pragma once
#include "ddns_framework.h"
#include "pch.h"

#include "config.h"
#include "api_requester.h"


using namespace HYDRA15::Union;

namespace HYDRA15::AliyunDDNSCpp::api_request
{
	// 发送具体的 api 请求
	// 在构造时自动设置请求参数。然后使用方法一键发送请求

	using api_requester = HYDRA15::AliyunDDNSCpp::api_requester_sdkv2;

	class recordid :public api_requester
	{
		const std::string domain;
		const std::string record;
		const std::string type;
	public:
		recordid(
			const std::string& d, // 域名
			const std::string& r, // 记录
			const std::string& t  // 类型
		);
		recordid() = delete;
		virtual ~recordid() = default;

		std::string get();
	};

	class addrecord :public api_requester
	{
		const std::string domain;
		const std::string record;
		const std::string type;
		const std::string ttl;
		const std::string value;
	public:
		addrecord(
			const std::string& d, // 域名
			const std::string& r, // 记录
			const std::string& t, // 类型
			const std::string& l, // ttl
			const std::string& v  // 值
		);
		addrecord() = delete;
		virtual ~addrecord() = default;

		std::string post();
	};

	class update :public api_requester
	{
		const std::string id;
		const std::string record;
		const std::string type;
		const std::string ttl;
		const std::string value;
	public:
		update(
			const std::string& i, // 记录id
			const std::string& r, // 记录
			const std::string& t, // 类型
			const std::string& l, // ttl
			const std::string& v  // 值
		);
		update() = delete;
		virtual ~update() = default;

		std::string post();
	};
}