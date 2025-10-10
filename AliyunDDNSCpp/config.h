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

		// http前缀
		static_string http = "http://";
		static_string https = "https://";

		// 配置项默认值
		static_string defaultRecord = "@";
		static_string defaultType = "A";
		static_uint defaultTtl = 600;

		// 默认超时
		static_uint httpTimeout = 10;

		// 阿里云 api url
		static_string aliyunApiUrl = "alidns.aliyuncs.com";

		
#ifdef _DEBUG
		static constexpr bool debug = true;
#else 
		static constexpr bool debug = false;
#endif

	}cfg;

	static struct registry_path
	{
		// 注册表路径
		static_string appRegtabRootPath = "HKEY_CURRENT_USER\\Software\\HYDRA15\\AliyunDDNScpp\\";
		static_string appRegtabAccesskeyidPath = "AccessKeyID";
		static_string appRegtabAccesskeysecretPath = "AccessKeySecret";
		static_string appRegtabLastipv4Path = "LastIPv4";
		static_string appRegtabLastipv6Path = "LastIPv6";
	}regPath;

	static struct json_config_keys
	{
		static_string ipurlsKey = "urls";
		static_string ipv4urlKey = "ipv4";
		static_string ipv6urlKey = "ipv6";

		static_string domainsLstKey = "domains";
		static_string domainKey = "domain";
		static_string recordKey = "record";
		static_string typeKey = "type";
		static_string ttlKey = "ttl";
	}jsonCfgKeys;

	static struct commands
	{
		struct command { HYDRA15::Union::framework::astring cmd; bool async; };
		command getip{ "getip",true };
		command getipv4{ "getipv4",true };
		command getipv6{ "getipv6",true };
	}cmds;

	static struct visualize
	{
		static_string logFileErrorFormat = "Failed to open log file: {}";
		static_string logSplit = "------------------------------------------------------------------";
		static_string accKeyLoadedSuccess = "Successfully loaded accessKey with ID {}.";
		static_string creatingConfigFile = "config.json not found, creating it now. Edit this file to configure the software.";
		static_string configFileLoadSuccess = "Successfully loaded config.json, containing 2 domains to be resolved.";
		static_string fetchip = "Starting to retrieve IP from {}.";
		static_string gotipv4 = "Current public IPv4: {}.";
		static_string gotipv6 = "Current public IPv6: {}.";


		// 报错消息
		static_string regkeyOpenFaildFormat = "Failed to open registry entry {} with code {}.";
		static_string regkeyWriteFaildFormat = "Failed to write registry entry {} with code {}.";
		static_string regkeyDeleteFaild = "Failed to delete registry entry {} with code {}.";
		static_string configfileLoadFaild = "Failed to load config.json.";
		static_string falidToLoadConfig = "Configuration failed to load correctly. It is recommended to use configuration mode when running for the first time.";
		static_string invalidDomain = "Invalid domain configuration entry.";
		static_string httpFailure = "HTTP request to {} failed with status code {}.";
		static_string httpFailureUnknown = "HTTP request to {} failed with unknown reason.";
		static_string recordNotFound = "Record {} not found.";
		static_string recordUpdateFailed = "Update record {} faild, return message: {}";
		static_string recordAddFailed = "Add record {} faild, return message: {}";
		static_string failedToSaveConfig = "Unable to save configuration, which may affect future usage.";

	}vslz;

	using domain_info = std::tuple<std::string, std::string, std::string, unsigned int>;	// domain, record, type, ttl
}

