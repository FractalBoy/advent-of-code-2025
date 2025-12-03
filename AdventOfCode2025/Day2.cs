namespace AdventOfCode2025
{
    public class Day2 : Day
    {
        public Day2()
        {
            DayNumber = 2;
        }

        private static IEnumerable<(long, long)> GetRanges(string input)
        {
            return input.Split(',')
            .Where(l => !string.IsNullOrWhiteSpace(l))
            .Select(l =>
            {
                var s = l.Split('-').Select(i => long.Parse(i));
                return (s.ElementAt(0), s.ElementAt(1));
            });
        }

        private static int numDigits(long number)
        {
            return (int)Math.Floor(Math.Log10(number)) + 1;
        }

        public override string Part1(string input)
        {
            var ranges = GetRanges(input);
            long sum = 0;

            foreach (var (start, end) in ranges)
            {
                int startDigits = numDigits(start);
                int endDigits = numDigits(end);

                long realStart = start;
                long realEnd = end;

                if (startDigits % 2 != 0)
                {
                    startDigits++;
                    realStart = (long)Math.Pow(10, startDigits - 1);
                }

                if (endDigits % 2 != 0)
                {
                    endDigits--;
                    realEnd = (long)Math.Pow(10, endDigits) - 1;
                }

                long startDivisor = (long)Math.Pow(10, startDigits / 2);
                long endDivisor = (long)Math.Pow(10, endDigits / 2);

                long startRange = realStart / startDivisor;
                long endRange = realEnd / endDivisor;

                for (long i = startRange; i <= endRange; i++)
                {
                    long digits = numDigits(startRange);
                    long number = (long)Math.Pow(10, digits) * i + i;

                    if (number >= start && number <= end)
                    {
                        sum += number;
                    }
                }
            }

            return sum.ToString();
        }

        public override string Part2(string input)
        {
            throw new NotImplementedException();
        }
    }
}