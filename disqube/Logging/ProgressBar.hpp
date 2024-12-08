#ifndef _PROGRESSBAR_H
#define _PROGRESSBAR_H

#include <functional>
#include <string>
#include <sstream>
#include <chrono>
#include <cmath>
#include <iostream>

#define BLOCK "\u2588" // Defines the block character used in the progress bar

namespace Qube::Logging
{
    class ProgressBar
    {
        private:
            using time_point = std::chrono::_V2::high_resolution_clock::time_point;

            const static int BAR_WIDTH = 50;

            int         start; // Start value of the iteration range
            int         stop;  // Stop value of the iteration range
            int         step;  // Step size of the iteration
            std::string msg;   // The message to show at the head of the bar
            
            int total       = 0;   // Total number of steps, set a runtime
            int current     = 0;   // Current number of steps, set a runtime
            double currperc = 0.0; // Current percentage of completing
            double dperc    = 0.0; // Delta percentage step
            
            time_point start_time; // Start time of the progress bar

            static void showBar(int completed, int total, double perc, 
                const std::string& msg, double etime);

        public:
            ProgressBar(int __start, int __stop, int __step, const std::string& __msg);

            template <typename _Callable, typename ..._Args>
            static void display(const int start, const int stop, const int step, 
                const std::string& msg, _Callable&& fn, _Args&&... args)
            {
                int total_steps = (stop - start) / step; // Total number of steps
                if ((stop - start) % step != 0) total_steps++; // Handle partial steps

                double dperc = 100.0 / total_steps; // Compute the delta percentage
                double currperc = 0.0; // Initial percentage
                int completed_steps = 0; // Initial number of completed steps

                // Initialize the start time of the progress bar
                auto start_time = std::chrono::high_resolution_clock::now();

                for (int idx = start; idx < stop; idx += step)
                {
                    // Invoke the user provided function that must have with
                    // required parameter, i.e., the iterator variable and
                    // optional arguments given as input to the current function
                    std::invoke(std::forward<_Callable>(fn), idx, std::forward<_Args>(args)...);

                    // Update the progress bar
                    currperc = currperc + dperc;
                    completed_steps++;
                    auto stop_time = std::chrono::high_resolution_clock::now();
                    std::chrono::duration<double> etime = stop_time - start_time;
                    ProgressBar::showBar(completed_steps, total_steps, currperc, 
                        msg, etime.count());
                }

                std::cout << std::endl; 
                std::cout << std::endl;
            }
    };
}

#endif