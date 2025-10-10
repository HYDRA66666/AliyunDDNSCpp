#include "ddns_framework.h"
#include "pch.h"

#include "initializer.h"
#include "config.h"

using namespace HYDRA15::AliyunDDNSCpp;
using namespace HYDRA15::Union;

int main(int argc, char* argv[])
{
	commander::Command::excute(cmds.getip.cmd.data());
	std::this_thread::sleep_for(std::chrono::seconds(100));
}