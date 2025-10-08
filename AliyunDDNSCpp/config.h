#pragma once
#include "framework.h"
#include "pch.h"

#include "config.h"

static struct config
{
	// 文件路径
	static_string logFilePath = ".\\logs\\";
	static_string logFileNameFormat = "{}.log";
	static_string configFilePath = ".\\config.json";
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

static struct visualize
{
	static_string logFileErrorFormat = "Failed to open log file: {}";
	static_string logSplit = "------------------------------------------------------------------";

	// 报错消息
	static_string regkeyOpenFaildFormat = "Failed to open registry entry {} with code {}.";
	static_string regkeyWriteFaildFormat = "Failed to write registry entry {} with code {}.";
	static_string regkeyDeleteFaild = "Failed to delete registry entry {} with code {}.";
	static_string falidToLoadConfig = "Configuration failed to load correctly. It is recommended to use configuration mode when running for the first time.";
	static_string failedToSaveConfig = "Unable to save configuration, which may affect future usage.";
	
}vslz;

