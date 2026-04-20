# UptimeLoggerDaemon

UptimeLoggerDaemon is a simple Linux daemon that periodically reads the system uptime from `/proc/uptime` and prints it in a human-readable `hours:minutes:seconds` format.

## Files

- `uptimeLoggerd.c` — Source code.
- `uptimeLoggerd` — Compiled executable.

## Functionality

- Installs handlers for `SIGINT` and `SIGTERM`.
- Every 5 seconds:
  - Opens `/proc/uptime`.
  - Reads the first value (uptime in seconds since boot).
  - Converts it to hours, minutes, and seconds.
  - Prints a line like:
    ```text
    Uptime: 1h 23m 45s
    ```
- Runs until a termination signal is received.

## Error Handling

- Exits with code `1` if `/proc/uptime` cannot be opened.
- Exits with code `2` if the contents of `/proc/uptime` cannot be read.

## Usage

```bash
./uptimeLoggerd
```

Stop it with `Ctrl+C` or by sending `SIGTERM` from another process.
