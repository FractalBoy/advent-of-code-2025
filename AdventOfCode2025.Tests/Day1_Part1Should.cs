namespace AdventOfCode2025.Tests
{
    public class Day1_Part1Should
    {
        [Fact]
        public void Part1_WithExampleInput_ReturnsExpectedResult()
        {
            string input = @"L68
L30
R48
L5
R60
L55
L1
L99
R14
L82
";
            Day1 day1 = new();
            string result = day1.Part1(input);
            Assert.Equal("3", result);
        }
    }
}