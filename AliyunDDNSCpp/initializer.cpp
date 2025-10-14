#include "initializer.h"
#include "pch.h"

extern bool sysworking;
extern std::mutex sysmtx;
extern std::condition_variable syscv;

namespace HYDRA15::AliyunDDNSCpp
{
	void initializer::uninstall()
	{
		lastIPv4.clear();
		lastIPv6.clear();
		accessKeyID.clear();
		accessKeySecret.clear();

		delete_registry_item(regPath.appRegtabLastipv4Path.data());
		delete_registry_item(regPath.appRegtabLastipv6Path.data());
		delete_registry_item(regPath.appRegtabAccesskeyidPath.data());
		delete_registry_item(regPath.appRegtabAccesskeysecretPath.data());
	}

	std::string initializer::get_registry_item(const std::string& item)
	{
		HKEY hKey;
		hkey_guard hg(hKey);
		std::string regItem = regPath.appRegtabRootPath.data() + item;

		LONG result = RegOpenKeyExA(HKEY_CURRENT_USER, regItem.data(), 0, KEY_READ, &hKey);
		if (result != ERROR_SUCCESS)
			throw std::runtime_error(std::format("Failed to open regiestry entry {} with code {}.", regItem, result));

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
			throw std::runtime_error(std::format("Faild to create registry entry {} with code {}.", regItem, result));

		result = RegSetValueExA(hKey, regItem.data(), 0, REG_SZ, (const BYTE*)value.data(), (DWORD)value.size() + 1);
		if (result != ERROR_SUCCESS) 
			throw std::runtime_error(std::format("Faild to save registry entry {} with code {}.", regItem, result));
	}

	void initializer::delete_registry_item(const std::string& item)
	{
		std::string regItem = regPath.appRegtabRootPath.data() + item;
		LONG result = RegDeleteTreeA(HKEY_CURRENT_USER, regItem.data());
		if (result != ERROR_SUCCESS && result != ERROR_FILE_NOT_FOUND)
			throw std::runtime_error(std::format("Failed to delete registry entry {} with code {}.", regItem, result));
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
			std::runtime_error e(std::format("Failed to open log file {}.", logFileName));

			// 检查并创建目录
			if (std::filesystem::exists(cfg.logFilePath.data()))
			{
				if (!std::filesystem::is_directory(cfg.logFilePath.data()))
					throw e;
			}
			else
				if (!std::filesystem::create_directory(cfg.logFilePath.data()))
					throw e;

			// 打开文件
			logFile.open(logFileName, std::ios::in | std::ios::out | std::ios::app);
			if (logFile.is_open())
			{
				pc.fredirect([this](const std::string& str) {logFile << str; logFile.flush(); });
				lgr.debug("Using log file {}.", logFileName);
			}
			else
				throw e;
		}
		catch (const std::exception& e) { lgr.warn(e.what()); }

		// 解析注册表配置
		try
		{
			accessKeyID = get_registry_item(regPath.appRegtabAccesskeyidPath.data());
			accessKeySecret = get_registry_item(regPath.appRegtabAccesskeysecretPath.data());

			lgr.info(std::format("Successfully loaded AccessKey with ID {}.", accessKeyID));
			regReady = true;
		}
		catch (const std::exception& e) { lgr.error(e.what()); }
		try { lastIPv4 = get_registry_item(regPath.appRegtabLastipv4Path.data()); lgr.debug("Using last ip {}.", lastIPv4); }
		catch (const std::exception& e) { lgr.warn(e.what()); }
		try { lastIPv6 = get_registry_item(regPath.appRegtabLastipv6Path.data()); lgr.debug("Using last ip {}.", lastIPv6); }
		catch (const std::exception& e) { lgr.warn(e.what()); }

		// 解析json配置
		try
		{
			std::runtime_error e("Failed to load config.json.");
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
				throw std::runtime_error("config.json not exist, creating template of it, edit it and restart this software.");
			}

			// 读取配置文件
			std::ifstream ifs(cfg.configFilePath.data(), std::ios::in);
			nlohmann::json j = nlohmann::json::parse(ifs);

			// 解析url
			try
			{
				ipv4url = j.at("urls").value("ipv4", "");
				lowcase(ipv4url);
				remove_first_substr(ipv4url, "http://");
				remove_first_substr(ipv4url, "https://");
				lgr.info("Using ipv4url {}.", ipv4url);
				ipv6url = j.at("urls").value("ipv6", "");
				lowcase(ipv6url);
				remove_first_substr(ipv6url, "http://");
				remove_first_substr(ipv6url, "https://");
				lgr.info("Using ipv6url {}.", ipv6url);
			}
			catch (const std::exception& e) { lgr.error(e.what()); }

			// 解析域名列表
			j = j.at("domains");
			for (const auto& i : j)
			{
				try
				{
					domain_info di;
					di.domain = i.at("domain");
					di.record = i.value("record", "@");
					std::string type = i.value("type", "A");
					di.ttl = i.value("ttl", 600);
					di.forceUpdate = i.value("force_refresh", false);
					// 检查和规范
					std::runtime_error e(std::format("Invalid domain config: {}.",di.domain));
					if (di.domain.empty())
						throw e;
					if (type == "A")
						di.type = domain_info::Type::A;
					else if (type == "AAAA")
						di.type = domain_info::Type::AAAA;
					else
						throw e;
					domains.push_back(di);
				}
				catch (const std::exception& e) { lgr.error(e.what()); }
			}

			lgr.info(std::format("Successfully loaded config.json, which contain(s) {} domains to be resolved.", domains.size()));
			domainsReady = true;
		}
		catch (const std::exception& e) { lgr.error(e.what());}
	}

	initializer::~initializer()
	{
		lastIPv4 = ipv4;
		lastIPv6 = ipv6;

		// 存储表项
		if(!accessKeyID.empty())
		{
			try 
			{ 
				set_registry_item(regPath.appRegtabAccesskeyidPath.data(), accessKeyID); 
				lgr.debug("Saving registry entry {} with data {}.", regPath.appRegtabAccesskeyidPath.data(), accessKeyID);
			}
			catch (const std::exception& e) { lgr.error(e.what()); lgr.error("Failed to save registry config."); }
		}
		if (!accessKeySecret.empty())
		{
			try 
			{ 
				set_registry_item(regPath.appRegtabAccesskeysecretPath.data(), accessKeySecret); 
				lgr.debug("Saving registry entry {} with data {}.", regPath.appRegtabAccesskeysecretPath.data(), accessKeyID);
			}
			catch (const std::exception& e) { lgr.error(e.what()); lgr.error("Failed to save registry config."); }
		}
		if (!lastIPv4.empty())
		{
			try 
			{ 
				set_registry_item(regPath.appRegtabLastipv4Path.data(), lastIPv4); 
				lgr.debug("Saving registry entry {} with data {}.", regPath.appRegtabLastipv4Path.data(), lastIPv4);
			}
			catch (const std::exception& e) { lgr.error(e.what()); lgr.error("Failed to save registry config."); }
		}
		if (!lastIPv6.empty())
		{
			try 
			{ 
				set_registry_item(regPath.appRegtabLastipv6Path.data(), lastIPv6); 
				lgr.debug("Saving registry entry {} with data {}.", regPath.appRegtabLastipv6Path.data(), lastIPv6);
			}
			catch (const std::exception& e) { lgr.error(e.what()); lgr.error("Failed to save registry config."); }
		}

		// 打印日志分隔符
		lgr.info("---------------------------------------------------------");

		// 关闭文件
		pc.sync_flush();
		pc.fredirect(nullptr);
		logFile.close();
	}

}
