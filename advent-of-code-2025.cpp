#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#include "advent_of_code.h"

using namespace AdventOfCode2025;

void doPart1(std::unordered_map<int, std::unique_ptr<Day>> &days, int day);
void doPart2(std::unordered_map<int, std::unique_ptr<Day>> &days, int day);

int main(int argc, char *argv[])
{
    std::string usage = "Usage: " + std::string(argv[0]) + " --day DAY [--part-1 | --part-2]";

    if (argc < 2)
    {
        std::cerr << usage << std::endl;
        return 1;
    }

    int day = -1;
    bool part1 = false;
    bool part2 = false;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg == "--day" && i + 1 < argc)
        {
            if (i + 1 >= argc)
            {
                std::cerr << usage << std::endl;
                return 1;
            }

            try
            {
                day = std::stoi(argv[++i]);
            }
            catch (...)
            {
                std::cerr << "Invalid day number." << std::endl;
                std::cerr << usage << std::endl;
                return 1;
            }
        }
        else if (arg == "--part-1")
        {
            part1 = true;
        }
        else if (arg == "--part-2")
        {
            part2 = true;
        }
        else
        {
            std::cerr << "Unknown argument: " << arg << std::endl;
            std::cerr << usage << std::endl;
            return 1;
        }
    }

    std::unordered_map<int, std::unique_ptr<Day>> days;
    days.emplace(1, std::make_unique<Day1>());

    if (days.find(day) == days.end())
    {
        std::cerr << "Day " << day << " is not implemented." << std::endl;
        return 1;
    }

    try
    {
        if (part1)
        {
            doPart1(days, day);
        }
        if (part2)
        {
            doPart2(days, day);
        }

        if (!part1 && !part2)
        {
            doPart1(days, day);
            doPart2(days, day);
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

void doPart1(std::unordered_map<int, std::unique_ptr<Day>> &days, int day)
{
    std::cout << days[day]->Part1() << std::endl;
}

void doPart2(std::unordered_map<int, std::unique_ptr<Day>> &days, int day)
{
    std::cout << days[day]->Part2() << std::endl;
}