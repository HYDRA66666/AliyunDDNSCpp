#pragma once
#include "ddns_framework.h"
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
        "           \"record\" : \"ddns\",\n"
        "           \"type\" : \"A\",\n"
        "           \"ttl\" : 600\n"
        "       },\n"
        "       {\n"
        "           \"domain\": \"example2.com\",\n"
        "           \"record\" : \"ddns\",\n"
        "           \"type\" : \"A\",\n"
        "           \"ttl\" : 600\n"
        "       }\n"
        "   ]\n"
        "}\n";
}