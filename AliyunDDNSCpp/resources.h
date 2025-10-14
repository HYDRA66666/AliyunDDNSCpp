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
        "           \"ttl\" : 600,\n"
		"			\"force_refresh\" : false\n"
        "       },\n"
        "       {\n"
        "           \"domain\": \"example2.com\",\n"
        "           \"record\" : \"ddns\",\n"
        "           \"type\" : \"A\",\n"
        "           \"ttl\" : 600,\n"
		"			\"force_refresh\" : false\n"
        "       }\n"
        "   ]\n"
        "}\n";

	static_string helpstr = ""
		"Usage: \n"
		"   Only one of the following arguments can be specified: \n"
		"	-c  Configuration mode : \n"
		"			run without arguments for interactive setup,\n"
		"			or provide two arguments[accessKeyId][accessKeySecret] for direct setup.\n"
		"	-s  Run silently\n"
		"   -d  Delete registry entries, used for software uninstallation.\n"
		"   -h  Print this help message\n";
}
