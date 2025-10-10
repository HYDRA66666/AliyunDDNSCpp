#pragma once
#include "framework.h"
#include "pch.h"

#include "ThreadLake.h"

namespace HYDRA15::Union::commander
{
    class GlobalThreadLake : public labourer::ThreadLake
    {
        // 单例模式
    private:
        GlobalThreadLake();
        GlobalThreadLake(const GlobalThreadLake&) = delete;
        GlobalThreadLake(GlobalThreadLake&&) = delete;

    public:
        ~GlobalThreadLake() = default;
        static GlobalThreadLake& get_instance();
    };
}