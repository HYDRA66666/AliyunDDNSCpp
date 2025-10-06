#include "pch.h"
#include "initializer.h"

namespace HYDRA15::AliyunDDNSCpp
{
	std::string initializer::get_registry_item(const std::string& item)
	{
		HKEY hKey;
		hkey_guard hg(hKey);
		std::string regItem = regPath.appRegtabRootPath.data() + item;

		LONG result = RegOpenKeyExA(HKEY_CURRENT_USER, regItem.data(), 0, KEY_READ, &hKey);
		if (result != ERROR_SUCCESS)
			throw std::runtime_error(std::format(vslz.regkeyOpenFaildFormat.data(), regItem, result));

		char buffer[64] = { 0 };
		DWORD bufferSize = sizeof(buffer);
		result = RegQueryValueExA(hKey, regItem.data(), nullptr, nullptr, (BYTE*)buffer, &bufferSize);
		return std::string(buffer);
	}

	void initializer::set_registry_item(const std::string& item, const std::string& value)
	{
		HKEY hKey;
		hkey_guard hg(hKey);
		std::string regItem = regPath.appRegtabRootPath.data() + item;

		LONG result = RegCreateKeyExA(HKEY_CURRENT_USER, regItem.data(), 0, nullptr, 0, KEY_WRITE,
			nullptr, &hKey, nullptr);
		if (result != ERROR_SUCCESS)
			throw std::runtime_error(std::format(vslz.regkeyOpenFaildFormat.data(), regItem, result));

		result = RegSetValueExA(hKey, regItem.data(), 0, REG_SZ, (const BYTE*)value.data(), (DWORD)item.size() + 1);
		if (result != ERROR_SUCCESS) 
			throw std::runtime_error(std::format(vslz.regkeyOpenFaildFormat.data(), regItem, result));
	}

	initializer::initializer()
	{
		// 初始化 commander 框架
		{	// 设置日志文件
			std::string logFileName = std::format(cfg.logFileFormat, Union::assistant::datetime::now_date_time("%Y-%m-%d"));
			logFile.open(logFileName, std::ios::in | std::ios::out | std::ios::app);
			if (!logFile.is_open())
				lgr.warn(logFileName);
			else
				pc.fredirect([this](const std::string& str) {logFile << str; logFile.flush(); });
		}

		// 解析配置
		try
		{
			accessKeyID = get_registry_item(regPath.appRegtabAccesskeyidPath.data());
			accessKeySecret = get_registry_item(regPath.appRegtabAccesskeysecretPath.data());
		}
		catch (const std::exception& e)
		{
			lgr.error(e.what());
			is_ready = false;
		}
		try { lastIPv4 = get_registry_item(regPath.appRegtabLastipv4Path.data()); }
		catch (const std::exception& e) { lgr.warn(e.what()); }
		try { lastIPv6 = get_registry_item(regPath.appRegtabLastipv6Path.data()); }
		catch (const std::exception& e) { lgr.warn(e.what()); }

	}

	initializer::~initializer()
	{
		// 存储表项
		if(!accessKeyID.empty())
		{
			try { set_registry_item(regPath.appRegtabAccesskeyidPath.data(), accessKeyID); }
			catch (const std::exception& e) { lgr.error(e.what()); lgr.error(vslz.failedToSaveConfig.data()); }
		}
		if (!accessKeySecret.empty())
		{
			try { set_registry_item(regPath.appRegtabAccesskeysecretPath.data(), accessKeyID); }
			catch (const std::exception& e) { lgr.error(e.what()); lgr.error(vslz.failedToSaveConfig.data()); }
		}
		if (!lastIPv4.empty())
		{
			try { set_registry_item(regPath.appRegtabLastipv4Path.data(), accessKeyID); }
			catch (const std::exception& e) { lgr.error(e.what()); lgr.error(vslz.failedToSaveConfig.data()); }
		}
		if (!lastIPv6.empty())
		{
			try { set_registry_item(regPath.appRegtabLastipv6Path.data(), accessKeyID); }
			catch (const std::exception& e) { lgr.error(e.what()); lgr.error(vslz.failedToSaveConfig.data()); }
		}
	}

	hkey_guard::hkey_guard(HKEY& k)
		: hkey(k)
	{
	}
	hkey_guard::~hkey_guard()
	{
		RegCloseKey(hkey);
	}
}
