#include <CommonLib/System/Metrics.hpp>

namespace sys = Lib::System;

int main()
{
    struct sys::SystemMetrics metrics;
    sys::collect(&metrics, 200);

    std::cout << "System Metrics Outcome: " << std::endl;
    std::cout << "Total CPU Usage: " << metrics.cpu_usage << " %" << std::endl;
    std::cout << "Physical Total RAM: " << metrics.pram_tot / (1024) << " KB" << std::endl;
    std::cout << "Physical Free RAM: " << metrics.pram_free / (1024) << " KB" << std::endl;
    std::cout << "Physical Virtual RAM: " << metrics.vram_tot / (1024) << " KB" << std::endl;

    return 0;
}