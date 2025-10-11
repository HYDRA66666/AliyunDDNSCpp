#include "initializer.h"
#include "pch.h"

extern bool sysworking;
extern std::mutex sysmtx;
extern std::condition_variable syscv;

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

		result = RegSetValueExA(hKey, regItem.data(), 0, REG_SZ, (const BYTE*)value.data(), (DWORD)value.size() + 1);
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
			{
				pc.fredirect([this](const std::string& str) {logFile << str; logFile.flush(); });
				lgr.debug(vslz.openlogfile.data(), logFileName);
			}
			else
				throw e;
		}
		catch (const std::exception& e) { lgr.warn(e.what()); }

		// 设置线程池
		cmd.threadpool(tl);

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
		try { lastIPv4 = get_registry_item(regPath.appRegtabLastipv4Path.data()); lgr.debug(vslz.loadlastip.data(), lastIPv4); }
		catch (const std::exception& e) { lgr.warn(e.what()); }
		try { lastIPv6 = get_registry_item(regPath.appRegtabLastipv6Path.data()); lgr.debug(vslz.loadlastip.data(), lastIPv6); }
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

			// 强制刷新
			forceUpdate = j.value(jsonCfgKeys.forceUpdate.data(), false);
			if (forceUpdate)
			{
				lgr.info(vslz.enableForceUpdate.data());
				lastIPv4 = "";
				lastIPv6 = "";
			}

			// 解析url
			try
			{
				ipv4url = j.at(jsonCfgKeys.ipurlsKey.data()).at(jsonCfgKeys.ipv4urlKey.data());
				lowcase(ipv4url);
				remove_first_substr(ipv4url, cfg.http.data());
				remove_first_substr(ipv4url, cfg.https.data());
				lgr.debug(vslz.useIpurl.data(), ipv4url);
				ipv6url = j.at(jsonCfgKeys.ipurlsKey.data()).at(jsonCfgKeys.ipv6urlKey.data());
				lowcase(ipv6url);
				remove_first_substr(ipv6url, cfg.http.data());
				remove_first_substr(ipv6url, cfg.https.data());
				lgr.debug(vslz.useIpurl.data(), ipv6url);
			}
			catch (const std::exception& e) { lgr.error(e.what()); }

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
					domains.push_back(di);
				}
				catch (const std::exception& e) { lgr.error(e.what()); }
			}

			lgr.info(std::format(vslz.configFileLoadSuccess.data(), domains.size()));
		}
		catch (const std::exception& e) { lgr.error(e.what()); is_ready = false; }

		// 注册控制函数
		cmd.regist_command(cmds.AliyunDDNSCpp.cmd.data(), cmds.AliyunDDNSCpp.async, startup);
		cmd.regist_default_command(cmds.AliyunDDNSCpp.async, startup);
		cmd.regist_command(cmds.help.cmd.data(), cmds.help.async, help);
		cmd.regist_command(cmds.config.cmd.data(), cmds.config.async, config);
		cmd.regist_command(cmds.silent.cmd.data(), cmds.silent.async, silent);
		cmd.regist_command(cmds.getip.cmd.data(), cmds.getip.async, getip);
		cmd.regist_command(cmds.getipv4.cmd.data(), cmds.getipv4.async, get_ipv4);
		cmd.regist_command(cmds.getipv6.cmd.data(), cmds.getipv6.async, get_ipv6);
		cmd.regist_command(cmds.updaterecord.cmd.data(), cmds.updaterecord.async, updaterecord);
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
				lgr.debug(vslz.savingRegEntry.data(), regPath.appRegtabAccesskeyidPath.data(), accessKeyID);
			}
			catch (const std::exception& e) { lgr.error(e.what()); lgr.error(vslz.failedToSaveConfig.data()); }
		}
		if (!accessKeySecret.empty())
		{
			try 
			{ 
				set_registry_item(regPath.appRegtabAccesskeysecretPath.data(), accessKeySecret); 
				lgr.debug(vslz.savingRegEntry.data(), regPath.appRegtabAccesskeysecretPath.data(), accessKeyID);
			}
			catch (const std::exception& e) { lgr.error(e.what()); lgr.error(vslz.failedToSaveConfig.data()); }
		}
		if (!lastIPv4.empty())
		{
			try 
			{ 
				set_registry_item(regPath.appRegtabLastipv4Path.data(), lastIPv4); 
				lgr.debug(vslz.savingRegEntry.data(), regPath.appRegtabLastipv4Path.data(), lastIPv4);
			}
			catch (const std::exception& e) { lgr.error(e.what()); lgr.error(vslz.failedToSaveConfig.data()); }
		}
		if (!lastIPv6.empty())
		{
			try 
			{ 
				set_registry_item(regPath.appRegtabLastipv6Path.data(), lastIPv6); 
				lgr.debug(vslz.savingRegEntry.data(), regPath.appRegtabLastipv6Path.data(), lastIPv6);
			}
			catch (const std::exception& e) { lgr.error(e.what()); lgr.error(vslz.failedToSaveConfig.data()); }
		}

		// 打印日志分隔符
		lgr.info(vslz.logSplit.data());

		// 关闭文件
		pc.sync_flush();
		pc.fredirect(nullptr);
		logFile.close();
	}

	void initializer::startup(const std::list<std::string>& args)
	{
		secretary::logger lgr = UNION_CREATE_LOGGER();
		if (args.size() == 1)	// 未加参数：如果初始化成功，则解析，否则进入配置流程
			if (get_instance().is_ready)
			{
				commander::Command::excute(cmds.silent.cmd.data());
				return;
			}
			else
			{
				commander::Command::excute(cmds.config.cmd.data());
				return;
			}
		else // 加参数：解析参数并执行
		{
			auto it = args.begin();
			it++;
			while (it != args.end())
			{
				if (*it == cmds.config.scmd.data())
				{
					it++;
					if (it == args.end())
					{
						commander::Command::excute(cmds.config.cmd.data());
						return;
					}
					else
					{
						auto p1 = it;
						it++;
						auto p2 = it;
						if (p2 == args.end() || p1->front() == '-' || p2->front() == '-')
						{
							lgr.error(vslz.invalidArgs.data());
							return;
						}
						else
						{
							commander::Command::excute(cmds.config.cmd.data() + std::string{ " " } + *p1 + std::string{ " " } + *p2);
							it++;
							return;
						}
					}
				}
				if (*it == cmds.help.scmd.data())
				{
					commander::Command::excute(cmds.help.cmd.data());
					it++;
					return;
				}
				if (*it == cmds.silent.scmd.data())
				{
					it++;
					if (!initializer::get_instance().is_ready)
						lgr.error(vslz.notConfiged.data());
					commander::Command::excute(cmds.silent.cmd.data());
					return;
				}
				lgr.error(vslz.invalidArgs.data());
				return;
			}
		}
	}

	void initializer::help(const std::list<std::string>&)
	{
		std::cout << resources::helpstr;
		sysworking = false;
		syscv.notify_all();
	}

	void initializer::config(const std::list<std::string>& args)
	{
		initializer& init = initializer::get_instance();
		secretary::logger lgr = UNION_CREATE_LOGGER();
		if (args.size() == 1)
		{
			init.accessKeyID = commander::Command::getline(vslz.inputAccki.data());
			init.accessKeySecret = commander::Command::getline(vslz.inputAccks.data());
			lgr.info(vslz.configComplete.data());
		}
		else if (args.size() == 3)
		{
			auto it = args.begin();
			it++;
			init.accessKeyID = *it;
			it++;
			init.accessKeySecret = *it;
			lgr.info(vslz.configComplete.data());
		}
		else lgr.error(vslz.invalidArgs.data());
		sysworking = false;
		syscv.notify_all();
	}

	void initializer::silent(const std::list<std::string>& args)
	{
		initializer& init = initializer::get_instance();
		secretary::logger lgr = UNION_CREATE_LOGGER();
		if (!init.is_ready)
		{
			lgr.error(vslz.notConfiged.data());
			return;
		}

		lgr.debug(vslz.startSilently.data());
		// 获取ip
		lgr.debug(vslz.startFetchIP.data());
		init.ipfetched = 0;
		commander::Command::excute(cmds.getip.cmd.data());
		while (init.ipfetched < 2)
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		lgr.debug(vslz.fetchIPComplete.data());

		// 解析地址
		lgr.debug(vslz.startResolvingDomains.data());
		int count = 0;
		std::list<std::string> argsFormat = { cmds.updaterecord.cmd.data() };
		for (const auto& domain : init.domains)
		{
			std::string ddomain = std::get<0>(domain);
			std::string record = std::get<1>(domain);
			std::string type = std::get<2>(domain);
			std::string ttl = std::to_string(std::get<3>(domain));
			// 判断ip变化
			if (type == "AAAA" && init.lastIPv6 == init.ipv6)
			{
				lgr.info(vslz.ipnotchange.data(), record + "." + ddomain);
				continue;
			}
			if (type == "A" && init.lastIPv4 == init.ipv4)
			{
				lgr.info(vslz.ipnotchange.data(), record + "." + ddomain);
				continue;
			}
			std::list<std::string> args = argsFormat;
			args.push_back(ddomain);	// domain
			args.push_back(record);	// record
			args.push_back(type);	// type
			args.push_back(ttl);	// ttl
			commander::Command::excute(args);
			count++;
		}
		while (init.resolved < count)
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
		lgr.debug(vslz.resolvedDomains.data());
		sysworking = false;
		syscv.notify_all();
	}

	void initializer::getip(const std::list<std::string>&)
	{
		initializer::get_instance().ipfetched = 0;
		commander::Command::excute(cmds.getipv4.cmd.data());
		commander::Command::excute(cmds.getipv6.cmd.data());
	}

	void initializer::get_ipv4(const std::list<std::string>&)
	{
		secretary::logger lgr = UNION_CREATE_LOGGER();
		std::string ipurl = initializer::get_instance().ipv4url;

		count_guard ifg(initializer::get_instance().ipfetched);
		lgr.debug(vslz.fetchip.data(), ipurl);
		httplib::SSLClient cli(ipurl);
		cli.set_connection_timeout(cfg.httpTimeout);
		auto res = cli.Get("/");
		if (res && res->status == 200)
		{
			lgr.info(vslz.gotipv4.data(), res->body);
			initializer::get_instance().ipv4 = res->body;
		}
		else if (res)
			throw std::runtime_error(std::format(
				vslz.httpFailure.data(),
				ipurl,
				res->status
			));
		else
			throw std::runtime_error(std::format(
				vslz.httpFailureUnknown.data(),
				ipurl
			));
	}

	void initializer::get_ipv6(const std::list<std::string>&)
	{
		secretary::logger lgr = UNION_CREATE_LOGGER();
		std::string ipurl = initializer::get_instance().ipv6url;

		count_guard ifg(initializer::get_instance().ipfetched);
		lgr.debug(vslz.fetchip.data(), ipurl);
		httplib::SSLClient cli(ipurl);
		cli.set_connection_timeout(cfg.httpTimeout);
		auto res = cli.Get("/");

		if (res && res->status == 200)
		{
			lgr.info(vslz.gotipv6.data(), res->body);
			initializer::get_instance().ipv6 = res->body;
		}
		else if (res)
			throw std::runtime_error(std::format(
				vslz.httpFailure.data(),
				ipurl,
				res->status
			));
		else
			throw std::runtime_error(std::format(
				vslz.httpFailureUnknown.data(),
				ipurl
			));
	}

	void initializer::updaterecord(const std::list<std::string>& args)
	{
		initializer& init = initializer::get_instance();
		secretary::logger lgr = UNION_CREATE_LOGGER();
		count_guard cg{ init.resolved };

		auto it = args.begin();
		std::string domain = *(++it);
		std::string record = *(++it);
		std::string type = *(++it);
		std::string ttl = *(++it);
		std::string value;

		if (type == "AAAA")
			value = init.ipv6;
		else if (type == "A")
			value = init.ipv4;
		else
		{
			lgr.error(vslz.invalidDomain.data());
			return;
		}

		lgr.debug(vslz.startResolvingDomain.data(), record + "." + domain);
		try
		{
			std::string recordid = api_request::recordid(domain, record, type).get();
			if (recordid.empty())
				recordid = api_request::addrecord(domain, record, type, ttl, value).post();
			else
				recordid = api_request::update(recordid, record, type, ttl, value).post();
			if (!recordid.empty())
				lgr.info(vslz.updateSuccess.data(), record + "." + domain, value);
			else
				lgr.error(vslz.updateFaild.data(), record + "." + domain);
		}
		catch (const std::exception& e) { lgr.error(e.what()); }
	}




	hkey_guard::hkey_guard(HKEY& k)
		: hkey(k)
	{
	}
	hkey_guard::~hkey_guard()
	{
		RegCloseKey(hkey);
	}

	count_guard::count_guard(std::atomic<int>& c)
		:count(c)
	{
	}

	count_guard::~count_guard()
	{
		count++;
	}
}
