#pragma once
#include "framework.h"
#include "pch.h"

namespace HYDRA15::AliyunDDNSCpp::resources
{
    static_string configJsonDemo = ""
        "{\n"
        "   \"urls\": {\n"
        "       \"ipv4\": \"https://api.ipify.org\",\n"
        "       \"ipv6\" : \"https://api6.ipify.org\"\n"
        "   },\n"
        "   \"domains\" : [\n"
        "       {\n"
        "           \"domain\": \"example.com\",\n"
        "           \"subdomain\" : \"ddns\",\n"
        "           \"type\" : \"A\",\n"
        "           \"ttl\" : \"60\"\n"
        "       },\n"
        "       {\n"
        "           \"domain\": \"example2.com\",\n"
        "           \"subdomain\" : \"ddns\",\n"
        "           \"type\" : \"A\",\n"
        "           \"ttl\" : \"60\"\n"
        "       }\n"
        "   ]\n"
        "}\n";
}