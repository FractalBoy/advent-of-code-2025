using Microsoft.Extensions.Configuration;

namespace AdventOfCode2025
{
    public abstract class Day : IDay
    {
        public abstract string Part1(string input);
        public abstract string Part2(string input);

        protected int DayNumber { get; set; }

        public async Task<string> GetInput()
        {
            string inputsDirectory = Path.Combine(Directory.GetCurrentDirectory(), "Inputs");

            if (!Directory.Exists(inputsDirectory))
            {
                Directory.CreateDirectory(inputsDirectory);
            }

            string inputFilePath = Path.Combine(inputsDirectory, $"day{DayNumber}.txt");

            if (File.Exists(inputFilePath))
            {
                return await File.ReadAllTextAsync(inputFilePath);
            }

            var config = new ConfigurationBuilder().SetBasePath(Directory.GetCurrentDirectory()).AddJsonFile("appsettings.json", true).Build();
            UriBuilder uriBuilder = new($"https://adventofcode.com/2025/day/{DayNumber}/input");
            HttpClient client = new();
            client.DefaultRequestHeaders.Add("Cookie", $"session={config["SessionId"]}");
            string input = await client.GetStringAsync(uriBuilder.Uri);

            await File.WriteAllTextAsync(inputFilePath, input);
            return input;
        }
    }
}