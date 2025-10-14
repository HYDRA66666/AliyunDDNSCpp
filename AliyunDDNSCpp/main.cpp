#include "ddns_framework.h"
#include "pch.h"

#include "initializer.h"
#include "config.h"
#include "resources.h"
#include "alidns_request.h"
#include "guards.h"

using namespace HYDRA15::AliyunDDNSCpp;
using namespace HYDRA15::Union;


// 主处理函数
static void help()
{
	std::cout << resources::helpstr;
}

static void start_config(std::list<std::string> args)	// 应当传入两个参数，分别为 accKeyID 和 accKeySec，或者没有参数，否则报错
{
	initializer& init = initializer::get_instance();
	secretary::logger lgr = UNION_CREATE_LOGGER();
	if (args.empty())
	{
		init.accessKeyID = secretary::ScanCenter::getline("AccessKeyID: ");
		init.accessKeySecret = secretary::ScanCenter::getline("AccessKeySecret: ");
	}
	else if (args.size() == 2)
	{
		init.accessKeyID = args.front();
		init.accessKeySecret = args.back();
	}
	else 
	{
		std::string line; 
		for (const auto& arg : args)
			line += arg;
		lgr.error("Bad arguments: " + line);
		help();
	}

	lgr.info("Configuration completed. Modify config.json and restart the application to perform domain name resolution.");
	return;
}

static void fetch_ip(domain_info::Type type, std::latch& lth)
{
	initializer& init = initializer::get_instance();
	secretary::logger lgr = UNION_CREATE_LOGGER();
	latch_guard lg(lth);

	std::string ipurl;
	std::string* ipstr = nullptr;
	switch (type)
	{
	case domain_info::Type::A:
		ipurl = init.ipv4url; ipstr = &init.ipv4; break;
	case domain_info::Type::AAAA:
		ipurl = init.ipv6url; ipstr = &init.ipv6; break;
	}

	lgr.debug("Starting fetching ip from {}", ipurl);
	httplib::SSLClient cli(ipurl);
	auto resp = cli.Get("/");

	if (!resp)
	{
		lgr.error("Failed to fetch ip from {}.", ipurl);
		return;
	}
	if (resp->status != 200)
	{
		lgr.error("Failed to fetch ip from {}, code: {}, message: \n{}", ipurl, resp->status, resp->body);
		return;
	}

	*ipstr = resp->body;
	lgr.info("Current public IP is {}", *ipstr);
}

static void resolve_domain(domain_info& domain, std::latch& lth)
{
	initializer& init = initializer::get_instance();
	secretary::logger lgr = UNION_CREATE_LOGGER();
	latch_guard lg(lth);

	std::string fullDomain = domain.record + "." + domain.domain;
	lgr.debug("Starting resolve domain {}", fullDomain);

	// 检查ip变化
	bool notChange = false;
	switch (domain.type)
	{
	case domain_info::Type::A:
		if (init.ipv4 == init.lastIPv4)notChange = true;
		break;
	case domain_info::Type::AAAA:
		if (init.ipv6 == init.lastIPv6)notChange = true;
		break;
	}
	if (notChange) { lgr.info("IP not change, skip domain {}", fullDomain); return; }

	// 获取id
	try { domain.recordID = api_request::recordid(domain).get(); }
	catch (const std::exception& e) { lgr.error("Failed to fetch recordID for domain {}", fullDomain); lgr.error(e.what()); return; }

	// 设置解析
	if (domain.recordID.empty())	// 为空，找不到记录，则创建记录
	{
		lgr.info("DNS record for domain {} not exist, creating...", fullDomain);
		try { domain.recordID = api_request::addrecord(domain).post(); }
		catch (const std::exception& e) { lgr.error("Failed to create DNS record for domain {}", fullDomain); lgr.error(e.what()); return; }
	}
	else
	{
		try { api_request::update(domain).post(); }
		catch(const std::exception& e) { lgr.error("Failed to update DNS record for domain {}", fullDomain); lgr.error(e.what()); return; }
	}
	lgr.info("Successfully updated DNS record for domain {}.", fullDomain);
	return;
}

static void resolve()
{
	initializer& init = initializer::get_instance();
	secretary::logger lgr = UNION_CREATE_LOGGER();

	lgr.debug("Starting Dynamic DNS task.");

	{
		std::latch latch(2);
		if (!init.ipv4url.empty())
			init.threadpool.submit(fetch_ip, domain_info::Type::A, std::ref(latch));
		if (!init.ipv4url.empty())
			init.threadpool.submit(fetch_ip, domain_info::Type::AAAA, std::ref(latch));
		latch.wait();
	}

	{
		std::latch latch(init.domains.size());
		for (auto& domain : init.domains)
			init.threadpool.submit(resolve_domain, domain, std::ref(latch));
		latch.wait();
	}

	lgr.info("Dynamic DNS update task completed.");
}

static void uninstall()
{
	initializer& init = initializer::get_instance();
	secretary::logger lgr = UNION_CREATE_LOGGER();

	try { init.uninstall(); lgr.info("Cleanup succeeded."); return; }
	catch (const std::exception& e) { lgr.error(e.what()); return; }
}




int main(int argc, char* argv[])
{
	std::list<std::string> args;
	if (argc > 0)
		for (int i = 1; i < argc; i++)
			args.push_back(argv[i]);

	initializer& init = initializer::get_instance();
	secretary::logger lgr = UNION_CREATE_LOGGER();

	// 没有参数：首次进配置模式，否则开始解析
	if (args.empty())
	{
		if (!init.regReady) { start_config(args); return 0; }
		if (!init.domainsReady) { lgr.error("Edit config.json to config this software."); return -1; }
		resolve(); return 0;
	}
	if (args.size() > 0)
	{
		if (args.front() == "-c") { args.pop_front(); start_config(args); return 0; }
		if (args.front() == "-s")
			if (init.regReady && init.domainsReady) { resolve(); return 0; }
			else { lgr.error("The software has not been configured correctly. Please refer to https://github.com/HYDRA66666/AliyunDDNSCpp for usage instructions."); return -1; }
		if (args.front() == "-d") { uninstall(); return 0; }
		if (args.front() == "-h") { help(); return 0; }
	}
	lgr.error("Unknown command.");
	help(); return -1;
}