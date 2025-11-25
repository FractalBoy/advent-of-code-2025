#include <memory>
#include <stdexcept>
#include <array>
#include <vector>
#include <fstream>
#include <unordered_map>

#include "advent_of_code.h"

using namespace AdventOfCode2025;

namespace AdventOfCode2025
{
    std::string Day::Part1()
    {
        std::string input = this->GetInput();
        return this->DoPart1(input);
    }

    std::string Day::Part2()
    {
        std::string input = this->GetInput();
        return this->DoPart2(input);
    }

    std::string Day::GetInput()
    {
        std::ifstream icache;
        const std::string path = "day" + std::to_string(this->dayNumber) + ".txt";

        icache.open(path);

        if (icache.is_open())
        {
            const auto size = icache.seekg(0, std::ios::end).tellg();
            icache.seekg(0, std::ios::beg);
            std::string content(size, '\0');
            icache.read(content.data(), size);
            return content;
        }

        std::unique_ptr<HttpsClient> client = std::make_unique<HttpsClient>();
        std::unique_ptr<HttpResponse> response = client->GetInput(this->dayNumber);
        int status = response->status;
        std::string body = response->body;

        if (status != 200)
        {
            throw std::runtime_error("HTTP request failed with status " + std::to_string(status) + ". Body: " + body);
        }

        std::ofstream ocache;
        ocache.open(path);

        ocache << body;

        return body;
    }
}