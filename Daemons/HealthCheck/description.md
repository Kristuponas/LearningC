# HealthCheck

HealthCheck is a simple TCP health checking daemon that periodically tests connectivity to one or more services (host:port pairs) using non-blocking socket connections.

It resolves hostnames, attempts TCP connections, and reports whether each target is reachable.

## Files

- `healthcheckd.c` — Source code for the health checker.
- `healthcheckd` — Compiled executable.
- `Makefile` — Build configuration.

## Functionality

HealthCheck:
- Accepts one or more `host:port` arguments on the command line.
- Resolves each host/port to one or more IP addresses using `getaddrinfo` [web:21][web:27].
- For each resolved address:
  - Prints basic information (socket type, address family, IP address).
  - Creates a TCP socket.
  - Switches the socket to non-blocking mode using `fcntl` [web:16][web:18][web:26].
  - Initiates a `connect` and waits for completion using `select` with a timeout.
  - Determines if the connection succeeded or failed using `getsockopt(SO_ERROR)`.

The program loops forever (until interrupted) and periodically repeats the checks for all configured hosts.

## How It Works

### 1. Argument Parsing

- Expects at least one argument:
  ```bash
  ./healthcheckd <service1:port1> [service2:port2] ...
  ```
- For each argument:
  - Splits on `:` to separate hostname and port.
  - Stores them into an array of `HostInfo` structures (up to 10 hosts).
  - Prints parsed host and port for confirmation.

### 2. Signal Handling

- Installs handlers for:
  - `SIGINT`
  - `SIGTERM`
- When a termination signal is received, a global `terminate` flag is set, causing the main loop to exit cleanly.

### 3. Host Checking Logic

For each host in each cycle:

1. Call `getaddrinfo` to resolve the hostname and port into one or more `addrinfo` entries [web:21][web:27].
2. For each `addrinfo` entry:
   - Print:
     - Socket type.
     - Address family (IPv4 or IPv6).
     - Human-readable IP address using `inet_ntop` and `INET6_ADDRSTRLEN` [web:22][web:28].
   - Create a TCP socket with `socket`.
   - Set the socket to non-blocking mode with `fcntl(F_SETFL, O_NONBLOCK)` [web:16][web:18][web:26].
   - Call `connect`:
     - If it returns `0` or `-1` with `errno == EINPROGRESS`, the connection is in progress.
   - Use `select` on the socket’s write set with a 3-second timeout.
     - If `select` times out, the host is considered unreachable.
   - Call `getsockopt(SOL_SOCKET, SO_ERROR)` to retrieve the final connection status.
     - If the error code is `0`, the connection succeeded and the host is considered **up**.
3. Close the socket and free the `addrinfo` list when done.

If any address connects successfully, the function prints “Connection successful” and returns `true` for that host.

### 4. Main Loop

- After initialization:
  - Iterates over all configured hosts.
  - Calls `check_host` on each one.
  - Sleeps for 10 seconds (`SLEEP_TIME`) between cycles.
- Continues until `SIGINT` or `SIGTERM` is received.

## Usage

```bash
./healthcheckd example.com:80 localhost:22 10.0.0.5:5432
```

This will:
- Continuously check:
  - HTTP on `example.com:80`
  - SSH on `localhost:22`
  - A service on `10.0.0.5:5432`
- Print connection details and whether each address is reachable.

## Exit Codes

- `0` — Success / normal termination.
- `1` — Too few arguments (no host:port provided).
- `2` — `getaddrinfo` failure.
- `3` — Socket creation failure.
- `4` — `fcntl` failure when setting non-blocking mode.
- `5` — Connection failure (early `connect` error).

## Build Instructions

```bash
make
```

This compiles `healthcheckd.c` into the `healthcheckd` executable.

## Notes

- Works with both IPv4 and IPv6 addresses.
- Uses non-blocking sockets plus `select` to avoid hanging on slow or unreachable hosts [web:16][web:18][web:26].
- Designed as a simple connectivity probe, not a full-featured monitoring system.
