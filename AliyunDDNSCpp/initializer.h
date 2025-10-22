#pragma once
#include "ddns_framework.h"
#include "pch.h"

#include "config.h"
#include "resources.h"
#include "guards.h"

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
		secretary::PrintCenter& pc = secretary::PrintCenter::get_instance();
		std::shared_ptr<labourer::ThreadLake> tl = std::make_shared<labourer::ThreadLake>(cfg.asyncThreads);

		// 配置项
	public:
		// 注册表配置项
		std::string accessKeyID;
		std::string accessKeySecret;

		// 获取 ip 的 url
		std::list<std::string> ipv4urls;
		std::list<std::string> ipv6urls;

		// 待解析的域名
		std::list<domain_info> domains;

		// 标志位
		bool regReady = false;
		bool domainsReady = false;

		//全局变量
	public:
		std::string ipv4;
		std::string ipv6;

		// 线程池
		labourer::ThreadLake threadpool{ cfg.asyncThreads };
		
		// 功能
	public:
		void uninstall();

		// 辅助函数
	private:
		static std::string get_registry_item(const std::string& item);
		static void set_registry_item(const std::string& item, const std::string& value);
		static void delete_registry_item(const std::string& item);

		static std::string& remove_first_substr(std::string& str, const std::string& substr);
	public:
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
	};

}