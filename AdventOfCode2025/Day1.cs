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

        public int Password { get; private set; } = 0;

        public void ApplyTurn(Turn turn)
        {
            Position += turn.Distance * (int)turn.Direction;
            Position %= 100;

            if (Position == 0)
            {
                Password++;
            }
        }
    }

    public class Day1 : Day
    {
        public Day1()
        {
            DayNumber = 1;
        }

        private IEnumerable<Turn> GetTurns(string input)
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

        public override string Part1(string input)
        {
            var turns = GetTurns(input);
            Dial dial = new();

            foreach (var turn in turns)
            {
                dial.ApplyTurn(turn);
            }

            return dial.Password.ToString();
        }

        public override string Part2(string input)
        {
            throw new NotImplementedException();
        }
    }
}