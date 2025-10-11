#include "ddns_framework.h"
#include "pch.h"

#include "initializer.h"
#include "config.h"

using namespace HYDRA15::AliyunDDNSCpp;
using namespace HYDRA15::Union;

bool sysworking = true;
std::mutex sysmtx;
std::condition_variable syscv;

int main(int argc, char* argv[])
{
	std::unique_lock ul(sysmtx);
	std::list<std::string> args;
	for (int i = 0; i < argc; i++)
		args.push_back(argv[i]);
	commander::Command::excute(args);
	while (sysworking == true)
		syscv.wait(ul);
}