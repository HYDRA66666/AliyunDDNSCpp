#include "ddns_framework.h"
#include "pch.h"

#include "guards.h"

namespace HYDRA15::AliyunDDNSCpp
{

	hkey_guard::hkey_guard(HKEY& k)
		: hkey(k)
	{
	}
	hkey_guard::~hkey_guard()
	{
		RegCloseKey(hkey);
	}

	count_guard::count_guard(std::atomic<int>& c)
		:count(c)
	{
	}

	count_guard::~count_guard()
	{
		count++;
	}

	latch_guard::latch_guard(std::latch& lth)
		:latch(lth)
	{

	}

	latch_guard::~latch_guard()
	{
		latch.count_down();
	}
}