namespace AdventOfCode2025
{
    enum Direction
    {
        L = -1,
        R = 1
    }

    class Turn
    {
        public Direction Direction { get; set; }
        public int Distance { get; set; }
    }

    class Dial
    {
        private int Position { get; set; } = 50;

        public int PasswordPart1 { get; private set; } = 0;
        public int PasswordPart2 { get; private set; } = 0;

        private void ApplyTurn(Turn turn)
        {
            for (int i = 0; i < turn.Distance; i++)
            {
                Position += (int)turn.Direction;
                Position %= 100;

                if (Position == 0)
                {
                    PasswordPart2++;
                }
            }

            if (Position == 0)
            {
                PasswordPart1++;
            }
        }

        public void ApplyTurns(IEnumerable<Turn> turns)
        {
            foreach (var turn in turns)
            {
                ApplyTurn(turn);
            }
        }
    }

    public class Day1 : Day
    {
        public Day1()
        {
            DayNumber = 1;
        }

        private static List<Turn> GetTurns(string input)
        {
            List<Turn> turns = [];

            foreach (string line in input.Split('\n'))
            {
                if (string.IsNullOrWhiteSpace(line))
                {
                    continue;
                }

                Turn turn = new();

                if (line.ElementAt(0) == 'L')
                {
                    turn.Direction = Direction.L;
                }
                else
                {
                    turn.Direction = Direction.R;
                }

                _ = int.TryParse(string.Join("", line.Skip(1)), out int count);
                turn.Distance = count;
                turns.Add(turn);
            }

            return turns;
        }

        private static Dial TurnDial(string input)
        {
            var turns = GetTurns(input);
            Dial dial = new();
            dial.ApplyTurns(turns);
            return dial;
        }

        public override string Part1(string input)
        {
            return TurnDial(input).PasswordPart1.ToString();
        }

        public override string Part2(string input)
        {
            return TurnDial(input).PasswordPart2.ToString();
        }
    }
}