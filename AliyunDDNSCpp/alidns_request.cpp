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
		if (!resp)
			throw std::runtime_error(std::format(
				vslz.httpFailureUnknown.data(),
				fullUrl
			));
		else if (resp->status != 200)
			throw std::runtime_error(std::format(
				vslz.httpFailure.data(),
				fullUrl,
				resp->status
			));

		nlohmann::json result = nlohmann::json::parse(resp->body);
		for (auto& rec : result["DomainRecords"]["Record"]) {
			if (rec.value("RR", "") == record && rec.value("Type", "") == type) {
				return rec.at("RecordId");
			}
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

	bool update::post()
	{
		auto resp = send_request();
		if (!resp)
			throw std::runtime_error(std::format(
				vslz.httpFailureUnknown.data(),
				fullUrl
			));
		else if (resp->status != 200)
			throw std::runtime_error(std::format(
				vslz.httpFailure.data(),
				fullUrl,
				resp->status
			));

		if (!nlohmann::json::parse(resp->body).contains("RecordId"))
			throw std::runtime_error(std::format(
				vslz.recordUpdateFailed.data(),
				id,
				resp->body
			));
		return true;
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

	bool addrecord::post()
	{
		auto resp = send_request();
		if (!resp)
			throw std::runtime_error(std::format(
				vslz.httpFailureUnknown.data(),
				fullUrl
			));
		else if (resp->status != 200)
			throw std::runtime_error(std::format(
				vslz.httpFailure.data(),
				fullUrl,
				resp->status
			));

		if (!nlohmann::json::parse(resp->body).contains("RecordId"))
			throw std::runtime_error(std::format(
				vslz.recordAddFailed.data(),
				record + domain,
				resp->body
			));
		return true;
	}

}

