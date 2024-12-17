#ifndef _METRICS_COLLECTOR_H
#define _METRICS_COLLECTOR_H

#include <iostream>
#include <chrono>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <thread>
#include <sys/sysinfo.h>

namespace Lib::System
{
    struct CpuTimes
    {
        long long user;    // Time spent in user mode
        long long nice;    // Time spent in user mode with low priority
        long long system;  // Time spent in kernel mode
        long long idle;    // Time spent idle
        long long iowait;  // Time spent waiting I/O to complete
        long long irq;     // Time spents servicing hardware interrupts
        long long softirq; // Time spents servicing software interrupts

        long long getTotal() { return user + nice + system + idle + iowait + irq + softirq; }
        long long getIdle() { return idle + iowait; }
    };

    struct MemoryUsage
    {
        long long total_ram;    // Total RAM in Bytes
        long long free_ram;     // Total Free RAM in Bytes
        long long virtual_ram;  // Total Virtual RAM
    };

    struct SystemMetrics
    {
        double cpu_usage;    // The percentage of total CPU usage
        long long pram_free; // The total amount of physical free RAM
        long long pram_tot;  // The total amount of physical RAM
        long long vram_tot;  // The total amount of virtual free RAM
    };

    // Returns the total user, system, idle and more other, times
    void getCpuTimes(struct CpuTimes* cpu);

    // Fetch the memory usage, total RAM, free RAM and total Virtual RAM
    void getMemoryUsage(struct MemoryUsage* mem);

    // Returns the percentage of cpu usage computed between two intervals
    double getCpuUsage(struct CpuTimes* cpu1, struct CpuTimes* cpu2, 
        const unsigned int interval_ms);

    // Fetch system metrics: cpu usage and memory availability
    void collect(struct SystemMetrics* metrics, const unsigned int interval_ms);
}

#endif