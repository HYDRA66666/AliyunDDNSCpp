#pragma once
#include "ddns_framework.h"
#include "pch.h"

namespace HYDRA15::AliyunDDNSCpp
{
	class hkey_guard
	{
		HKEY& hkey;
	public:
		hkey_guard(HKEY& k);
		~hkey_guard();
	};

	class count_guard
	{
		std::atomic<int>& count;
	public:
		count_guard(std::atomic<int>& c);
		~count_guard();
	};

	class latch_guard
	{
		std::latch& latch;
	public:
		latch_guard(std::latch& lth);
		~latch_guard();
	};
}
