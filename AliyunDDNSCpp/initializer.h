#pragma once
#include "ddns_framework.h"
#include "pch.h"

#include "config.h"
#include "resources.h"
#include "alidns_request.h"

using namespace HYDRA15::Union;



namespace HYDRA15::AliyunDDNSCpp 
{
	// 程序启动时配置初始化，结束时清理，同时存储全局变量
	// 配置内容：
	//   框架对象：Command，Printcenter
	//   日志输出
	//   全局配置项：accessKey，lastIP，待解析域名
	// 清理内容：
	//   保存配置项：accessKey，lastIP
	class initializer
	{
		// 私有变量
	private:
		std::fstream logFile;

		// 系统对象
	private:
		secretary::logger lgr{ "Initializer" };
		commander::Command& cmd = commander::Command::get_instance();
		secretary::PrintCenter& pc = secretary::PrintCenter::get_instance();
		std::shared_ptr<labourer::ThreadLake> tl = std::make_shared<labourer::ThreadLake>(cfg.asyncThreads);

		// 配置项
	public:
		// 注册表配置项
		std::string accessKeyID;
		std::string accessKeySecret;
		std::string lastIPv4;
		std::string lastIPv6;

		// 强制刷新
		bool forceUpdate = false;

		// 获取 ip 的 url
		std::string ipv4url;
		std::string ipv6url;

		// 待解析的域名
		std::list<domain_info> domains;
		std::atomic<int> resolved = 0;

		//全局变量
	public:
		// 当前ip
		std::atomic<int> ipfetched = 0;
		std::string ipv4;
		std::string ipv6;
		

		// 标志位
	private:
		bool is_ready = true;

		// 辅助函数
	private:
		static std::string get_registry_item(const std::string& item);
		static void set_registry_item(const std::string& item, const std::string& value);
		static void delete_registry_item(const std::string& item);

		static std::string& remove_first_substr(std::string& str, const std::string& substr);
		static std::string& lowcase(std::string& str);
		
		// 单例模式
	private:
		static initializer instance;
	public:
		static initializer& get_instance();

	private:
		initializer();
	public:
		~initializer();

		// 指令处理函数
	public:
		static void startup(const std::list<std::string>& args);
		static void help(const std::list<std::string>&);
		static void config(const std::list<std::string>& args);
		static void silent(const std::list<std::string>& args);
		static void getip(const std::list<std::string>&);
		static void get_ipv4(const std::list<std::string>&);
		static void get_ipv6(const std::list<std::string>&);
		static void updaterecord(const std::list<std::string>& args);
	};

	class hkey_guard
	{
		HKEY& hkey;
	public:
		hkey_guard(HKEY& k);
		~hkey_guard();
	};

	class count_guard
	{
		std::atomic<int>& count;
	public:
		count_guard(std::atomic<int>& c);
		~count_guard();
	};
}