#include "ProgressBar.hpp"

void Qube::Logging::ProgressBar::showBar(
    int completed, int total, double perc, const std::string &msg, double etime
) {
    int progress = (completed * BAR_WIDTH) / total;

    // Display the progress bar
    std::cout << "\r" << msg << " [";
    for (int i = 0; i < BAR_WIDTH; i++)
    {
        if (i < progress) std::cout << BLOCK;
        else std::cout << "-";
    }
    
    std::cout << "] " << perc << " % <ELAPS " << etime << " sec>";
    std::cout << std::flush; 
}

Qube::Logging::ProgressBar::ProgressBar(
    int __start, int __stop, int __step, const std::string &__msg
) : start(__start), stop(__stop), step(__step), msg(__msg)
{
    this->total = (this->stop - this->start) / this->step;
    if ((stop - start) % step != 0) this->total++;

    this->dperc = 100.0 / this->total;
    this->start_time = std::chrono::high_resolution_clock::now();
}
