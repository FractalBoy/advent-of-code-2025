using AdventOfCode2025;
using System.CommandLine;

Option<int> dayOption = new("--day", "-d")
{
    Description = "The day of the Advent of Code to run (1-12)",
    Required = true,
};

dayOption.Validators.Add(result =>
{
    int day = result.GetValue(dayOption);

    if (day < 1)
    {
        result.AddError("Day number must be >= 1");
    }

    if (day > 12)
    {
        result.AddError("Day number must be <= 12");
    }
});

Option<bool> part1Option = new("--part-1", "-p1")
{
    Description = "Run part 1 of the day's challenge",
    Required = false
};

Option<bool> part2Option = new("--part-2", "-p2")
{
    Description = "Run part 1 of the day's challenge",
    Required = false
};

RootCommand rootCommand = new();
rootCommand.Options.Add(dayOption);
rootCommand.Options.Add(part1Option);
rootCommand.Options.Add(part2Option);

var parseResult = rootCommand.Parse(args);

if (parseResult.Errors.Count != 0)
{
    foreach (var error in parseResult.Errors)
    {
        Console.Error.WriteLine(error);
    }

    return 1;
}

int day = parseResult.GetValue(dayOption);
bool part1 = parseResult.GetValue(part1Option);
bool part2 = parseResult.GetValue(part2Option);

if (!part1 && !part2)
{
    part1 = true;
    part2 = true;
}

string dayClassName = $"Day{day}";
Type? dayType = typeof(IDay).Assembly.GetTypes()
    .FirstOrDefault(t => t.Name == dayClassName && typeof(IDay).IsAssignableFrom(t) && !t.IsAbstract);

if (dayType == null)
{
    Console.Error.WriteLine($"Day {day} not yet implemented");
    return 1;
}

Day? dayInstance = Activator.CreateInstance(dayType) as Day;
if (dayInstance == null)
{
    Console.Error.WriteLine($"Day {day} not yet implemented");
    return 1;
}

string input = await dayInstance.GetInput();

if (part1)
{
    Console.WriteLine(dayInstance.Part1(input));
}

if (part2)
{
    Console.WriteLine(dayInstance.Part2(input));
}

return 0;