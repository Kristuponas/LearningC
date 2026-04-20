# FileStatusChange

FileStatusChange is a simple file monitoring daemon that watches a specific file or path for changes using the Linux inotify subsystem.

It is useful for testing and demonstrating real-time detection of file events such as modifications, creations, and deletions.

## Files

- `watcher.c` — Source code for the file watcher.
- `watcher` — Compiled executable.
- `test.txt` — Example file used to test change detection.

## Functionality

The watcher process monitors a given path and prints messages when certain file events occur. The events are detected using the inotify API from the Linux kernel, which provides real-time notifications for filesystem changes [web:1][web:12][web:14].

Monitored events include:
- File creation.
- File deletion.
- File access.
- File modification.
- File closed after write.
- File moved.

The program runs in an infinite loop and reacts whenever inotify reports one or more events.

## How It Works

1. Validates that a path argument is provided on the command line.
2. Extracts the base path name from the given path string.
3. Initializes an inotify instance with `inotify_init` [web:12].
4. Adds a watch on the specified path with a mask composed of:
   - `IN_CREATE`
   - `IN_DELETE`
   - `IN_ACCESS`
   - `IN_CLOSE_WRITE`
   - `IN_MODIFY`
   - `IN_MOVE_SELF` [web:1][web:12]
5. Enters an infinite loop:
   - Blocks on `read` from the inotify file descriptor until an event occurs [web:10][web:12].
   - Iterates over all `struct inotify_event` records in the buffer.
   - Matches the event mask and prints a corresponding human-readable message.

Example messages:
- `File created.`
- `File deleted.`
- `File accessed.`
- `File written and closed.`
- `File modified.`
- `File moved.`

## Usage

```bash
./watcher <path_to_watch>
```

Example:

```bash
./watcher ./test.txt
```

This command starts the watcher and monitors `test.txt` for changes. Any relevant file operation on `test.txt` will trigger a message on standard output.

## Exit Codes

The program uses specific exit codes to report different error conditions:

- `0` — Success.
- `1` — Too few arguments (no path provided).
- `2` — Base path not defined.
- `3` — Failed to initialize inotify.
- `4` — Failed to add watch on the file.
- `5` — Error reading from inotify instance.

## Notes

- This program is designed for Linux systems that support the inotify API (kernel 2.6.13 or later) [web:12][web:14].
- The process runs in the foreground and does not currently handle termination signals gracefully.
- Intended for debugging, experimentation, or educational use to understand file event notifications.
