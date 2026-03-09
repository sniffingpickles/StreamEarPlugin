# StreamEar OBS Plugin

Open-source OBS Studio plugin that captures the final audio mix and sends it to the StreamEar sidecar over localhost TCP.

## Features

- Captures OBS final audio output (alerts, TTS, music, game audio)
- Encodes as AAC frames with PTS timestamps
- Lock-free ring buffer for zero-copy audio transfer
- TCP sender with automatic reconnection
- No outbound network connections (localhost only)
- Cross-platform: Windows, macOS, Linux

## Building

### Prerequisites

- CMake 3.16+
- OBS Studio development headers (libobs)
- C compiler (GCC, Clang, or MSVC)

### Build

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

### Install

Copy the built plugin to your OBS plugins directory:

- **macOS**: `~/Library/Application Support/obs-studio/plugins/`
- **Linux**: `~/.config/obs-studio/plugins/`
- **Windows**: `C:\Program Files\obs-studio\obs-plugins\64bit\`

## Usage

1. Install the plugin
2. In OBS: Settings → Output → Add "StreamEar Audio Relay"
3. Set sidecar port (default: 19400)
4. Start the sidecar, then start streaming

## Architecture

```
OBS Audio Mix → AAC Encoder → Ring Buffer → TCP Sender → localhost:19400
                                (lock-free)
```

## License

MIT License — see LICENSE file.
