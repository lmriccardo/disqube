#ifndef _TEST_HPP
#define _TEST_HPP

#pragma once

#include <iostream>
#include <cstring>

namespace Test
{
    template <typename T>
    void assert_eq(T val1, T val2)
    {
        if (val1 != val2)
        {
            std::cerr << "Error: Output is ";
            std::cerr << val1 << " Instead of " << val2;
            std::cerr << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    template <typename T>
    void assert_neq(T val1, T val2)
    {
        if (val1 == val2)
        {
            std::cerr << "Error: Output ";
            std::cerr << val1 << " should not be equal to " << val2;
            std::cerr << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    void assert_eq_str(const char* val1, const char* val2, const std::size_t n)
    {
        if (strncmp(val1, val2, n) != 0)
        {
            std::cerr << "Error: Output is ";
            std::cerr << val1 << " Instead of " << val2;
            std::cerr << std::endl;
            exit(EXIT_FAILURE);
        }
    }
}

#endif