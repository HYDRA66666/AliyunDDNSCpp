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

	std::string& initializer::remove_first_substr(std::string& str, const std::string& substr)
	{
		auto it = str.find(substr);
		if (it != str.npos)
			str.erase(it, substr.size());
		return str;
	}

	std::string& initializer::lowcase(std::string& str)
	{
		for (auto& c : str)
			if (c >= 0x41 && c <= 0x5A)
				c = c + 0x20;
		return str;
	}

	initializer initializer::instance;

	initializer& initializer::get_instance()
	{
		return instance;
	}

	initializer::initializer()
	{
		secretary::log::debug(cfg.debug);

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

			lgr.info(std::format(vslz.accKeyLoadedSuccess.data(), accessKeyID));
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
			std::runtime_error e(vslz.configfileLoadFaild.data());
			// 检查文件
			if (std::filesystem::exists(cfg.configFilePath.data()))
			{
				if (std::filesystem::is_directory(cfg.configFilePath.data()))
					throw e;
			}
			else
			{
				std::ofstream of(cfg.configFilePath.data(), std::ios::out);
				if (!of.is_open())
					throw e;
				of << resources::configJsonDemo.data();
				of.close();
				throw std::runtime_error(vslz.creatingConfigFile.data());
			}

			// 读取配置文件
			std::ifstream ifs(cfg.configFilePath.data(), std::ios::in);
			nlohmann::json j = nlohmann::json::parse(ifs);

			// 解析url
			ipv4url = j.at(jsonCfgKeys.ipurlsKey.data()).at(jsonCfgKeys.ipv4urlKey.data());
			lowcase(ipv4url);
			remove_first_substr(ipv4url, cfg.http.data());
			remove_first_substr(ipv4url, cfg.https.data());
			ipv6url = j.at(jsonCfgKeys.ipurlsKey.data()).at(jsonCfgKeys.ipv6urlKey.data());
			lowcase(ipv6url);
			remove_first_substr(ipv6url, cfg.http.data());
			remove_first_substr(ipv6url, cfg.https.data());

			// 解析域名列表
			j = j[jsonCfgKeys.domainsLstKey.data()];
			for (const auto& i : j)
			{
				try
				{
					domain_info di{
						i.at(jsonCfgKeys.domainKey.data()),
						i.value(jsonCfgKeys.recordKey.data(),cfg.defaultRecord.data()),
						i.value(jsonCfgKeys.typeKey.data(),cfg.defaultType.data()),
						i.value(jsonCfgKeys.ttlKey.data(),cfg.defaultTtl)
					};
					// 检查和规范
					if (std::get<0>(di).empty())
						std::runtime_error e(vslz.invalidDomain.data());
					if (std::get<2>(di) != "A" && std::get<2>(di) != "AAAA")
						std::runtime_error e(vslz.invalidDomain.data());
				}
				catch (const std::exception& e) { lgr.error(e.what()); }
			}

			lgr.info(std::format(vslz.configFileLoadSuccess.data(), domains.size()));
		}
		catch (const std::exception& e) { lgr.error(e.what()); is_ready = false; }

		// 注册控制函数
		cmd.regist(cmds.getip.cmd.data(), cmds.getip.async, command_controller::getip);
		cmd.regist(cmds.getipv4.cmd.data(), cmds.getipv4.async, command_controller::get_ipv4);
		cmd.regist(cmds.getipv6.cmd.data(), cmds.getipv6.async, command_controller::get_ipv6);
		
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
