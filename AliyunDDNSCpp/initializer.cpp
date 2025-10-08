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

	void initializer::delete_registry_item(const std::string& item)
	{
		std::string regItem = regPath.appRegtabRootPath.data() + item;
		LONG result = RegDeleteTreeA(HKEY_CURRENT_USER, regItem.data());
		if (result != ERROR_SUCCESS && result != ERROR_FILE_NOT_FOUND)
			throw std::runtime_error(std::format(vslz.regkeyDeleteFaild.data(), regItem, result));
	}

	initializer initializer::instance;

	initializer& initializer::get_instance()
	{
		return instance;
	}

	initializer::initializer()
	{
		// 初始化 commander 框架
		try
		{	// 设置日志文件
			std::string logFileName = cfg.logFilePath.data() + std::format(cfg.logFileNameFormat.data(), Union::assistant::datetime::now_date_time("%Y-%m-%d"));
			std::runtime_error e(std::format(vslz.logFileErrorFormat.data(), logFileName));

			// 检查并创建目录
			if (std::filesystem::exists(cfg.logFilePath.data()))
			{
				if (!std::filesystem::is_directory(cfg.logFilePath.data()))
					throw e;
			}
			else
				if (!std::filesystem::create_directories(cfg.logFilePath.data()))
					throw e;

			// 打开文件
			logFile.open(logFileName, std::ios::in | std::ios::out | std::ios::app);
			if (logFile.is_open())
				pc.fredirect([this](const std::string& str) {logFile << str; logFile.flush(); });
			else
				throw e;
		}
		catch (const std::exception& e) { lgr.warn(e.what()); }

		// 解析注册表配置
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

		// 解析json配置
		try
		{
			// 检查并打开文件
		}
		catch (const std::exception& e) { lgr.error(e.what()); is_ready = false; }
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

		// 打印日志分隔符
		lgr.info(vslz.logSplit.data());

		// 关闭文件
		pc.sync_flush();
		pc.fredirect(nullptr);
		logFile.close();
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
