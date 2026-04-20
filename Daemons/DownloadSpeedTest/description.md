# DownloadSpeedTest

DownloadSpeedTest is a simple network benchmarking tool that measures download speed by retrieving a test file from Cloudflare’s speed test service.

## Functionality

The program downloads a file of a user-specified size and measures:
- Download speed (MB/s)
- Total download time
- Amount of data downloaded

Based on the results, it classifies the connection speed into categories.

## How It Works

- Generates a download URL using Cloudflare:
  https://speed.cloudflare.com/__down?bytes=<size_in_bytes>
- Uses libcurl to perform the download
- Discards received data (only measures performance, not storage)
- Collects statistics using curl APIs

## Speed Ratings

The connection is classified based on measured performance:

- Very fast
- Fast
- Medium
- Slow
- Very slow

The rating depends on how long it takes to download the specified file size.

## Usage

```
./speedtestd <size_in_MB>

Example: ./speedtestd 50
```

This downloads a 50 MB test file and evaluates the connection speed.

## Files

- speedtestd.c — source code
- speedtestd — compiled executable
- Makefile — build configuration

## Build Instructions

```
make
```

## Dependencies

- libcurl
- OpenSSL (libssl, libcrypto)

## Error Handling

- Exits if invalid arguments are provided
- Exits if download fails
- Exits if HTTP response is not 200 OK

## Notes

- Requires internet connection
- Uses Cloudflare’s public speed test service
- Does not store downloaded data (discarded during transfer)
- Intended for quick, approximate speed testing rather than precise benchmarking
