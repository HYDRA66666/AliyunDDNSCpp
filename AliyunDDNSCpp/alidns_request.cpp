#include "alidns_request.h"
#include "pch.h"

namespace HYDRA15::AliyunDDNSCpp::api_request
{
	recordid::recordid(domain_info di)
		:domainInfo(di)
	{
		params["Action"] = "DescribeDomainRecords";
		params["DomainName"] = domainInfo.domain;
		params["RRKeyWord"] = domainInfo.record;
		params["TypeKeyWord"] = domainInfo.type == domain_info::Type::A ? "A" : "AAAA";
	}

	std::string recordid::get()
	{
		auto resp = send_request();

		nlohmann::json result = nlohmann::json::parse(resp->body);
		for (auto& rec : result["DomainRecords"]["Record"]) {
			if (rec.value("RR", "") == domainInfo.record && rec.value("Type", "") == type && rec.value("DomainName", "") == domainInfo.domain)
				return rec.at("RecordId");
		}
		return "";
	}

	update::update(domain_info di)
		:domainInfo(di)
	{
		initializer& init = initializer::get_instance();
		params["Action"] = "UpdateDomainRecord";
		params["RecordId"] = domainInfo.recordID;
		params["RR"] = domainInfo.record;
		params["Type"] = domainInfo.type == domain_info::Type::A ? "A" : "AAAA";
		params["Value"] = domainInfo.type == domain_info::Type::A ? init.ipv4 : init.ipv6;
		params["TTL"] = std::to_string(domainInfo.ttl);
	}

	std::string update::post()
	{
		auto resp = send_request();
		nlohmann::json j = nlohmann::json::parse(resp->body);

		if (!j.contains("RecordId"))
			throw std::runtime_error(std::format(
				"Failed to update domain {}, response code: {}, message: \n{}.",
				domainInfo.record + "." + domainInfo.domain,
				resp->status,
				resp->body
			));
		return j.at("RecordId");
	}

	addrecord::addrecord(domain_info di)
		:domainInfo(di)
	{
		initializer& init = initializer::get_instance();
		params["Action"] = "AddDomainRecord";
		params["DomainName"] = domainInfo.domain;
		params["RR"] = domainInfo.record;
		params["Type"] = domainInfo.type == domain_info::Type::A ? "A" : "AAAA";
		params["Value"] = domainInfo.type == domain_info::Type::A ? init.ipv4 : init.ipv6;
		params["TTL"] = std::to_string(domainInfo.ttl);
	}

	std::string addrecord::post()
	{
		auto resp = send_request();
		nlohmann::json j = nlohmann::json::parse(resp->body);

		if (!j.contains("RecordId"))
			throw std::runtime_error(std::format(
				"Failed to creat record {}, response code: {}, message: \n{}.",
				domainInfo.record + "." + domainInfo.domain,
				resp->status,
				resp->body
			));
		return j.at("RecordId");
	}

}

