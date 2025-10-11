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
		static_string appRegtabRootPath = "Software\\HYDRA15\\AliyunDDNScpp\\";
		static_string appRegtabAccesskeyidPath = "AccessKeyID";
		static_string appRegtabAccesskeysecretPath = "AccessKeySecret";
		static_string appRegtabLastipv4Path = "LastIPv4";
		static_string appRegtabLastipv6Path = "LastIPv6";
	}regPath;

	static struct json_config_keys
	{
		static_string forceUpdate = "force_update";

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
		struct command 
		{ 
			HYDRA15::Union::framework::astring scmd;
			HYDRA15::Union::framework::astring cmd; 
			bool async; 
		};
		command AliyunDDNSCpp{ "","AliyunDDNSCpp",false };
		command help{ "-h","help",false };
		command config{ "-c","config",false };	// 带两个参数：keyid和keysecret
		command silent{ "-s","silent",true };
		command getip{ "","getip",true };
		command getipv4{ "","getipv4",true };
		command getipv6{ "","getipv6",true };
		command updaterecord{ "","update",true };	// 带一个参数：要解析的域名
	}cmds;

	static struct visualize
	{
		static_string logFileErrorFormat = "Failed to open log file: {}";
		static_string logSplit = "------------------------------------------------------------------";
		static_string accKeyLoadedSuccess = "Loaded accessKey with ID {}.";
		static_string loadlastip = "Loaded the previously resolved IP: {}.";
		static_string enableForceUpdate = "Forced update has been initiated; all domain records will be updated.";
		static_string openlogfile = "Using log file: {}";
		static_string inputAccki = "Enter accessKeyID: ";
		static_string inputAccks = "Enter accessKeySecret: ";
		static_string configComplete = "Configuration completed. Modify config.json and restart the application to perform domain name resolution.";
		static_string useIpurl = "Using ip url: {}";
		static_string creatingConfigFile = "config.json not found, creating it now. Edit this file to configure the software.";
		static_string configFileLoadSuccess = "Successfully loaded config.json, containing {} domains to be resolved.";
		static_string startSilently = "Start working silently.";
		static_string startFetchIP = "Start fetching IP.";
		static_string fetchIPComplete = "Fetch ip completed.";
		static_string startResolvingDomains = "Start resolving domains.";
		static_string startResolvingDomain = "Start resolving domain {}";
		static_string resolvedDomains = "All domains resolved.";
		static_string fetchip = "Starting to retrieve IP from {}.";
		static_string gotipv4 = "Current public IPv4: {}.";
		static_string gotipv6 = "Current public IPv6: {}.";
		static_string httpresponse = "HTTP request {} response code: {}. body: \n{}";
		static_string ipnotchange = "IP not change, skip record {}.";
		static_string updateSuccess = "Record {} successfully updated to {}.";
		static_string updateFaild = "Failed to update record {}.";
		static_string savingRegEntry = "Registry entry {} has been stored as {}.";


		// 报错消息
		static_string regkeyOpenFaildFormat = "Failed to open registry entry {} with code {}.";
		static_string regkeyWriteFaildFormat = "Failed to write registry entry {} with code {}.";
		static_string regkeyDeleteFaild = "Failed to delete registry entry {} with code {}.";
		static_string configfileLoadFaild = "Failed to load config.json.";
		static_string falidToLoadConfig = "Configuration failed to load correctly.";
		static_string invalidDomain = "Invalid domain configuration entry.";
		static_string notConfiged = "The program is not properly configured and cannot be executed.";
		static_string invalidArgs = "Invalid arguments, use -h for help.";
		static_string httpFailure = "HTTP request to {} failed with status code {}, message: {}.";
		static_string httpFailureUnknown = "HTTP request to {} failed with unknown reason.";
		static_string recordNotFound = "Record {} not found.";
		static_string recordUpdateFailed = "Update record {} faild, return message: {}";
		static_string recordAddFailed = "Add record {} faild, return message: {}";
		static_string failedToSaveConfig = "Unable to save configuration, which may affect future usage.";

	}vslz;

	using domain_info = std::tuple<std::string, std::string, std::string, unsigned int>;	// domain, record, type, ttl
}

