#pragma once
#include <chrono>
#include <string>
#include <iostream>

using namespace std::chrono;
class Timer
{
    using dms = duration<double, std::ratio<1, 1000>>;

private:
    time_point<steady_clock> start, end;
    std::string name;

public:
    Timer(const std::string& text = "") : start(steady_clock::now()), name(text)
    {
    }

    void setStart()
    {
        start = steady_clock::now();
    }

    void printDuration()
    {
        end = steady_clock::now();
        std::cout << name << " took: " << duration_cast<dms>(end - start).count() << " ms\n";
    }

    void printDuration(const std::string& text)
    {
        end = steady_clock::now();
        std::cout << text << " : " << duration_cast<dms>(end - start).count() << " ms\n";
    }
};

