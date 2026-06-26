# bf-a-day

Implementing a [Brainfuck](https://esolangs.org/wiki/Brainfuck) interpreter in a different programming language every day, starting from scratch each time with no reference to previous implementations.

The goal is to get comfortable enough with the implementation in C that it becomes muscle memory, then use BF as a consistent yardstick to get a real feel for each new language — how it handles memory, I/O, control flow, and error conditions.

See [LANGUAGES.md](LANGUAGES.md) for the full planned language list.

## Progress

| Day | Language |
|-----|----------|
| 1   | C        |
| 2   | C        |
| 3   | C        |
| 4   | Python   |
| 5   | Lua      |
| 6   | Ruby     |

## Structure

Each day lives in its own directory with whatever build artifacts that language needs. The implementation is always written fresh.

```
dayN/
├── bf.c        # (or equivalent for the language of that day)
├── Makefile    # (or equivalent)
└── bf          # compiled binary if applicable
```

## Tests

Tests are written in pytest and automatically scan any directory matching `day*`, running the full test suite against each day's implementation (there may be problems with this and I don't think it works as of right now, but i'll figure something out).

```sh
pytest
```

Test cases in `tests/` cover standard BF programs, edge cases, and error conditions including unmatched brackets and out-of-bounds pointer movement.

## Rules

- No reference to previous implementations while writing
- No BF libraries
- Standard BF behavior: 8-bit cells, wrapping arithmetic, dynamic tape

## License

GPL v3