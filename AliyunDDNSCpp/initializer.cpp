#include "pch.h"
#include "initializer.h"

namespace HYDRA15::AliyunDDNSCpp
{
	initializer::initializer(int argc, char* argv[])
	{
		// 拼接命令行参数
		for (int i = 0; i < argc; ++i)
		{
			cmdline.append(argv[i]);
			if (i < argc - 1)
				cmdline.append(" ");
		}

		// 初始化 commander 框架
		{	// 设置日志文件
			std::string logFileName = std::format(cfg.logFileFormat, Union::assistant::datetime::now_date_time("%Y-%m-%d"));
			logFile.open(logFileName, std::ios::in | std::ios::out | std::ios::app);
			if (!logFile.is_open())
				lgr.warn(logFileName);
			else
				pc.fredirect([this](const std::string& str) {logFile << str; logFile.flush(); });
		}
	}
}
