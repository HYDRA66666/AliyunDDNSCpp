#pragma once

// cpphttplib 启动ssl支持
#define CPPHTTPLIB_OPENSSL_SUPPORT

// 常用类型
#define static_string static constexpr std::string_view

// 配置项
#include <string_view>
static struct config
{
	// 文件路径
	static_string logFileFormat = "\\logs\\{}.log";
	static_string configFilePath = "config.json";

	// 注册表路径
	static_string appRegtabRootPath = "HKEY_CURRENT_USER\\Software\\HYDRA15\\AliyunDDNScpp\\";
	static_string appRegtabAccesskeyidPath = "AccessKeyID";
	static_string appRegtabAccesskeysecretPath = "AccessKeySecret";
	static_string appRegtabLastipv4Path = "LastIPv4";
	static_string appRegtabLastipv6Path = "LastIPv6";
}cfg;

