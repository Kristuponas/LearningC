# CPUnMemoryMonitor

CPUnMemoryMonitor is a Linux daemon that periodically reads and displays system memory and CPU statistics using the `/proc` filesystem.

The daemon runs in a loop and updates system statistics every 10 seconds until it receives a termination signal.

### Memory Monitoring
- Reads `/proc/meminfo`
- Extracts:
  - Total memory (MemTotal)
  - Available memory (MemAvailable)

### CPU Monitoring
- Reads `/proc/stat`
- Extracts CPU time values:
  - User
  - Nice
  - System
  - Idle
  - I/O wait
  - IRQ
  - SoftIRQ

All CPU values are measured in jiffies (1/100th of a second).

## Signal Handling
The process handles:
- SIGINT
- SIGTERM

On receiving these signals, it exits cleanly.

## Files
- monitor.c — source code
- monitord — compiled executable

## Execution
The daemon prints system statistics to standard output every 10 seconds.

## Notes
- Works only on Linux systems with `/proc` filesystem
- Does not run as a background daemon (no fork)
