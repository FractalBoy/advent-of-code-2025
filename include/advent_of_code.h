#pragma once

#include <string>

namespace AdventOfCode2025
{
    class Day
    {
    public:
        virtual ~Day() = default;
        std::string Part1();
        std::string Part2();

    protected:
        int dayNumber;

    private:
        std::string GetInput();
        virtual std::string DoPart1(std::string &) = 0;
        virtual std::string DoPart2(std::string &) = 0;
    };

    class Day1 : public Day
    {
    public:
        Day1();
        ~Day1() = default;

    private:
        std::string DoPart1(std::string &) override;
        std::string DoPart2(std::string &) override;
    };
}