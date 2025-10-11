#include "alidns_request.h"
#include "pch.h"

namespace HYDRA15::AliyunDDNSCpp::api_request
{
	recordid::recordid(const std::string& d, const std::string& r, const std::string& t)
		:domain(d), record(r), type(t)
	{
		params["Action"] = "DescribeDomainRecords";
		params["DomainName"] = d;
		params["RRKeyWord"] = r;
		params["TypeKeyWord"] = t;
	}

	std::string recordid::get()
	{
		auto resp = send_request();

		nlohmann::json result = nlohmann::json::parse(resp->body);
		for (auto& rec : result["DomainRecords"]["Record"]) {
			if (rec.value("RR", "") == record && rec.value("Type", "") == type && rec.value("DomainName", "") == domain)
				return rec.at("RecordId");
		}
		return "";
	}

	update::update(const std::string& i, const std::string& r, const std::string& t, const std::string& l, const std::string& v)
		:id(i), record(r), type(t), ttl(l), value(v)
	{
		params["Action"] = "UpdateDomainRecord";
		params["RecordId"] = i;
		params["RR"] = r;
		params["Type"] = t;
		params["Value"] = v;
		params["TTL"] = l;
	}

	std::string update::post()
	{
		auto resp = send_request();
		nlohmann::json j = nlohmann::json::parse(resp->body);

		if (!j.contains("RecordId"))
			throw std::runtime_error(std::format(
				vslz.recordUpdateFailed.data(),
				id,
				resp->body
			));
		return j.at("RecordId");
	}

	addrecord::addrecord(const std::string& d, const std::string& r, const std::string& t, const std::string& l, const std::string& v)
		:domain(d), record(r), type(t), ttl(l), value(v)
	{
		params["Action"] = "AddDomainRecord";
		params["DomainName"] = d;
		params["RR"] = r;
		params["Type"] = t;
		params["Value"] = v;
		params["TTL"] = l;
	}

	std::string addrecord::post()
	{
		auto resp = send_request();
		nlohmann::json j = nlohmann::json::parse(resp->body);

		if (!j.contains("RecordId"))
			throw std::runtime_error(std::format(
				vslz.recordAddFailed.data(),
				record + domain,
				resp->body
			));
		return j.at("RecordId");
	}

}

