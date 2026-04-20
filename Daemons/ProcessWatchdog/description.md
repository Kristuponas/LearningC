# ProcessWatchdog

ProcessWatchdog is a simple Linux watchdog daemon that monitors one or more named processes and restarts them if they stop running.

It is designed to supervise long-running programs such as the included `dummy` test process, which runs indefinitely in the background until terminated.

## Files

- `watchdogd.c` — Source code for the watchdog daemon.
- `watchdogd` — Compiled executable.
- `dummy.c` — Source code for a simple test process.
- `dummy` — Compiled test executable.
- `Makefile` — Build configuration for both binaries.

## Functionality

The watchdog accepts one or more process commands as command-line arguments and continuously checks whether those processes are running.

For each watched process, it:
- Stores the original command used to start it.
- Derives the process name from the command path.
- Uses `pgrep -x` to search for a running process with the exact name.
- Reads `/proc/<pid>/status` to obtain process details such as name, state, and PID.
- Detects whether the process has stopped since the last check.
- Restarts the process automatically if it was previously running and is no longer found.

The watchdog checks all configured processes every 5 seconds.

## How It Works

### 1. Startup

- Requires at least one process name or command as an argument.
- Installs signal handlers for:
  - `SIGINT`
  - `SIGTERM`
- Initializes a watchlist for up to 10 processes.

### 2. Process Detection

For each process in the watchlist:
- The program runs:
  ```bash
  pgrep -x <process_name>
  ```
- If a PID is found:
  - Marks the process as running.
  - Opens `/proc/<pid>/status`.
  - Parses fields such as:
    - `Name:`
    - `State:`
    - `Pid:`
- If no PID is found:
  - Marks the process as not running.
  - Prints that the process was not found.

### 3. Restart Logic

If a process was running in the previous cycle but is no longer running in the current cycle, the watchdog treats it as dead.

It then:
1. Prints a message that the process died.
2. Calls `fork()` to create a child process.
3. In the child:
   - Calls `setsid()` so the restarted process gets its own session.
   - Executes the original command with `execvp()`.
4. In the parent:
   - Prints the restarted PID.
   - Stores the new PID in the watchlist.

This allows the restarted child to continue running even if the watchdog later exits.

### 4. Termination

The watchdog runs in a loop until it receives `SIGINT` or `SIGTERM`, after which it exits cleanly.

## Usage

Run a process in the background first, for example:

```bash
./dummy &
```

Start the watchdog:

```bash
./watchdogd ./dummy
```

You can also watch multiple processes:

```bash
./watchdogd ./dummy another_process
```

If a watched process is killed, the watchdog will detect that it disappeared and will attempt to restart it automatically.

## Included Test Program

The included `dummy` program is a minimal infinite loop:

```c
int main(void)
{
    while(1)
        sleep(1);
}
```

It exists only to provide a simple background process for testing the watchdog.

## Build Instructions

```bash
make
```

This builds both:
- `watchdogd`
- `dummy`

## Exit Codes

- `0` — Success.
- `1` — Invalid usage / no process argument provided.
- `2` — `popen` failed.
- `3` — `pclose` failed.
- `4` — Failed to open `/proc/<pid>/status`.
- `5` — Reserved for `fclose` error handling.
- `6` — `fork` failed.
- `7` — `execvp` failed.

## Notes

- Designed for Linux systems with the `/proc` filesystem.
- Uses process names for lookup, so multiple processes with the same name may affect detection.
- Intended as a simple watchdog example rather than a production-grade supervisor.
- The restart command uses the original argument passed to the watchdog, so executable paths should be correct.
