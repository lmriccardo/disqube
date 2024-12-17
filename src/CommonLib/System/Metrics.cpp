#include "Metrics.hpp"

void Lib::System::getCpuTimes(CpuTimes *cpu)
{
    FILE* fd = fopen("/proc/stat", "r");

    fscanf(fd, "cpu %llu %llu %llu %llu %llu %llu %llu",
        &cpu->user, &cpu->nice, &cpu->system,
        &cpu->idle, &cpu->iowait, &cpu->irq,
        &cpu->softirq);

    fclose(fd);
}

void Lib::System::getMemoryUsage(MemoryUsage *mem)
{
    struct sysinfo mem_info;

    // Fetch system information
    if (sysinfo(&mem_info) != 0)
    {
        printf("[Metrics::%s] Failed to retrieve memory information.\n", __FUNCTION__);
        throw std::runtime_error("Error");
    }

    mem->total_ram = mem_info.totalram * mem_info.mem_unit;
    mem->free_ram = mem_info.freeram * mem_info.mem_unit;
    mem->virtual_ram = (mem_info.totalram + mem_info.totalswap) * mem_info.mem_unit;
}

double Lib::System::getCpuUsage(CpuTimes *cpu1, CpuTimes *cpu2, const unsigned int interval_ms)
{
    // Collect times for the first interval
    getCpuTimes(cpu1);

    // Sleep the required interval
    std::this_thread::sleep_for(std::chrono::milliseconds(interval_ms));

    // Collect times for the second interval
    getCpuTimes(cpu2);

    // Calculate the total time and deltas
    long long total1 = cpu1->getTotal();
    long long total2 = cpu2->getTotal();
    long long idle_time1 = cpu1->getIdle();
    long long idle_time2 = cpu2->getIdle();

    long long total_delta = total2 - total1;
    long long idle_delta = idle_time2 - idle_time1;

    double cpu_usage = 100.0 * (total_delta - idle_delta) / total_delta;
    return cpu_usage;
}

void Lib::System::collect(SystemMetrics *metrics, const unsigned int interval_ms)
{
    // Collect CPU usage
    struct CpuTimes cpu1, cpu2;
    double cpu_usage = getCpuUsage(&cpu1, &cpu2, interval_ms);

    // Collect RAM information
    struct MemoryUsage mem;
    getMemoryUsage(&mem);

    // Store the collected information into the output structure
    metrics->cpu_usage = cpu_usage;
    metrics->pram_free = mem.free_ram;
    metrics->pram_tot = mem.total_ram;
    metrics->vram_tot = mem.virtual_ram;
}
