namespace AdventOfCode2025.Tests
{
    public class Day2_Part1Should
    {
        [Fact]
        public void Part1_WithExampleInput_ReturnsExpectedResult()
        {
            string input = "11-22,95-115,998-1012,1188511880-1188511890,222220-222224,1698522-1698528,446443-446449,38593856-38593862,565653-565659,824824821-824824827,2121212118-2121212124\n";
            Day2 day2 = new();
            string result = day2.Part1(input);
            Assert.Equal("1227775554", result);
        }
    }
}