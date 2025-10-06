#pragma once
#include "framework.h"
#include "pch.h"

#include <hydra15/Union/Command.h>
#include <hydra15/Union/logger.h>
#include <hydra15/Union/datetime.h>



namespace HYDRA15::AliyunDDNSCpp 
{

	class initializer
	{
		static struct visualize
		{
			static_string logFileErrorFormat = "Failed to open log file: {}";
		}vslz;

	private:
		std::string cmdline;
		Union::secretary::logger lgr{ "Initializer" };

		Union::commander::Command& cmd = Union::commander::Command::get_instance();
		Union::secretary::PrintCenter& pc = Union::secretary::PrintCenter::get_instance();

		std::fstream logFile;
	public:
		initializer(int argc, char* argv[]);
		~initializer();
	};
}