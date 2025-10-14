#pragma once
#include "ddns_framework.h"
#include "pch.h"

namespace HYDRA15::AliyunDDNSCpp
{
	static struct config
	{
		// 文件路径
		static_string logFilePath = ".\\logs\\";
		static_string logFileNameFormat = "{}.log";
		static_string configFilePath = ".\\config.json";

		// 线程池数量
		static_uint asyncThreads = 4;

		// 配置项默认值
		static_string defaultRecord = "@";
		static_string defaultType = "A";
		static_uint defaultTtl = 600;

		// 默认超时
		static_uint httpTimeout = 10;

		// 阿里云 api url
		static_string aliyunApiUrl = "alidns.aliyuncs.com";

		bool debug = HYDRA15::Union::debug;

	}cfg;

	static struct registry_path
	{
		// 注册表路径
		static_string appRegtabRootPath = "Software\\HYDRA15\\AliyunDDNScpp\\";
		static_string appRegtabAccesskeyidPath = "AccessKeyID";
		static_string appRegtabAccesskeysecretPath = "AccessKeySecret";
		static_string appRegtabLastipv4Path = "LastIPv4";
		static_string appRegtabLastipv6Path = "LastIPv6";
	}regPath;

	// 存储带解析域名的结构
	struct domain_info
	{
		std::string recordID;
		std::string domain;
		std::string record;
		unsigned long long ttl;
		enum class Type :char { A = 1, AAAA = 2 }type;
		bool forceRefresh;
	};
}

