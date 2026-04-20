# SimpleHelloPrint

SimpleHelloPrint is a very basic daemon-style program that repeatedly prints a message to standard error once per second.

It was one of the first daemons for experimenting with a simple main loop and signal handling.

## Files

- `hellod.c` — Source code.
- `hellod` — Compiled executable.

## Functionality

- Installs handlers for `SIGINT` and `SIGTERM`.
- Runs an infinite loop until a termination signal is received.
- Prints `Hello!` every second to `stderr`.

## How It Works

1. `init_service()` sets up signal handlers that toggle a global `terminate` flag.
2. `run_main_loop()`:
   - Loops while `terminate` is false.
   - Prints `Hello!` followed by a newline.
   - Sleeps for 1 second between prints.
3. When a signal is received, `terminate` becomes true and the loop exits, allowing `main()` to return.

## Usage

```bash
./hellod
```

Stop it with `Ctrl+C` or by sending `SIGTERM` from another process.
