#include "api_requester.h"
#include "pch.h"

namespace HYDRA15::AliyunDDNSCpp
{
	std::string api_requester_sdkv2::percent_encode(const std::string& url)
	{
        std::ostringstream escaped;
        escaped.fill('0');
        escaped << std::hex << std::uppercase;
        for (unsigned char c : url) {
            if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
                escaped << static_cast<char>(c);
            }
            else {
                escaped << '%' << std::setw(2) << static_cast<int>(c);
            }
        }
        return escaped.str();
	}

    std::string api_requester_sdkv2::base64_encode(const std::vector<unsigned char>& data)
    {
        std::vector<unsigned char> data = data;
        BIO* b64 = BIO_new(BIO_f_base64());
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
        BIO* bmem = BIO_new(BIO_s_mem());
        b64 = BIO_push(b64, bmem);
        BIO_write(b64, data.data(), static_cast<int>(data.size()));
        BIO_flush(b64);
        BUF_MEM* bptr;
        BIO_get_mem_ptr(b64, &bptr);
        std::string encoded(bptr->data, bptr->length);
        BIO_free_all(b64);
        return encoded;
    }

    std::string api_requester_sdkv2::hmac_sha1_base64(const std::string& key, const std::string& data)
    {
        std::vector<unsigned char> digest(EVP_MAX_MD_SIZE);
        unsigned int len;
        HMAC(EVP_sha1(),
            reinterpret_cast<const unsigned char*>(key.data()), key.size(),
            reinterpret_cast<const unsigned char*>(data.data()), data.size(),
            digest.data(), &len);
        digest.resize(len);
        return base64_encode(digest);
    }

    std::string api_requester_sdkv2::generate_noce()
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 15);
        std::stringstream ss;
        ss << std::hex;
        for (int i = 0; i < 16; ++i) ss << dis(gen);
        return ss.str();
    }

    void api_requester_sdkv2::calculate_signature()
    {
        std::list<std::string> keys;
        for (const auto& kv : params) keys.push_back(kv.first);
        std::sort(keys.begin(), keys.end());

        std::ostringstream canonical;
        bool first = true;
        for (const auto& key : keys)
        {
            if (!first) canonical << "&";
            canonical << percent_encode(key) << "=" << percent_encode(params.at(key));
            first = false;
        }

        std::string stringToSign = "GET&%2F&" + percent_encode(canonical.str());
        params["Signature"] = hmac_sha1_base64(accessKeySecret + "&", stringToSign);
    }

    api_requester_sdkv2::api_requester_sdkv2()
        :accessKeyID(initializer::get_instance().accessKeyID)
        , accessKeySecret(initializer::get_instance().accessKeySecret)
    {
        params = {
            {"Version", "2015-01-09"},
            {"Format", "JSON"},
            {"SignatureMethod", "HMAC-SHA1"},
            {"SignatureVersion", "1.0"},
            {"AccessKeyId", accessKeyID},
        };
    }

    api_requester_sdkv2::api_requester_sdkv2(std::unordered_map<std::string, std::string> ps)
        :accessKeyID(initializer::get_instance().accessKeyID)
        , accessKeySecret(initializer::get_instance().accessKeySecret)
    {
        params = {
            {"Version", "2015-01-09"},
            {"Format", "JSON"},
            {"SignatureMethod", "HMAC-SHA1"},
            {"SignatureVersion", "1.0"},
            {"AccessKeyId", accessKeyID},
        };
        for (const auto& [k, v] : ps)
            params[k] = v;
    }

    httplib::Result api_requester_sdkv2::send_request()
    {
        params["SignatureNonce"] = generate_noce();
        params["Timestamp"] = assistant::datetime::now_date_time("%Y-%m-%dT%H:%M:%SZ", 0);

        calculate_signature();

        std::string url = "alidns.aliyuncs.com";

        std::ostringstream urp;
        urp << "/?";

        bool first = true;
        for (const auto& [k,v] : params) {
            if (!first) urp << "&";
            urp << k << "=" << percent_encode(v);
            first = false;
        }

        fullUrl = cfg.https.data() + url + urp.str();

        httplib::SSLClient cli(url);
        return cli.Get(urp.str());
    }


}