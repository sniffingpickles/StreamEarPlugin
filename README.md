# StreamEar OBS Plugin

Open-source OBS Studio plugin that captures the final audio mix and sends it to the StreamEar sidecar over localhost TCP.

## Features

- Captures OBS final audio output (alerts, TTS, music, game audio)
- Encodes as AAC frames with PTS timestamps
- Lock-free ring buffer for zero-copy audio transfer
- TCP sender with automatic reconnection
- No outbound network connections (localhost only)
- Auto-starts when streaming begins, auto-stops when streaming ends
- Manual toggle via Tools menu

## Install

### Windows

1. Download the latest build artifact from [GitHub Actions](https://github.com/sniffingpickles/StreamEarPlugin/actions)
2. Extract the zip — you'll get an `obs-streamear` folder
3. Copy the **entire** `obs-streamear` folder to:
   ```
   %APPDATA%\obs-studio\plugins\
   ```
   So you end up with:
   ```
   %APPDATA%\obs-studio\plugins\obs-streamear\bin\64bit\obs-streamear.dll
   %APPDATA%\obs-studio\plugins\obs-streamear\data\locale\en-US.ini
   ```
4. Restart OBS Studio

### macOS

Copy the `obs-streamear` folder to:
```
~/Library/Application Support/obs-studio/plugins/
```

### Linux

Copy the `obs-streamear` folder to:
```
~/.config/obs-studio/plugins/
```

## Usage

Once installed and OBS is restarted:

- **Automatic**: The relay starts automatically when you begin streaming and stops when you stop streaming
- **Manual**: Go to **Tools → StreamEar Audio Relay** to toggle the relay on/off at any time
- Check the OBS log for `[StreamEar]` messages to confirm it's working

The relay sends audio to `localhost:19400` where the StreamEar sidecar should be listening.

## Building from Source

### Prerequisites

- CMake 3.16+
- OBS Studio development headers (libobs + obs-frontend-api)
- C compiler (GCC, Clang, or MSVC)

### Build

```bash
mkdir build && cd build
cmake ..
cmake --build .
```

## Architecture

```
OBS Audio Mix → AAC Encoder → Ring Buffer → TCP Sender → localhost:19400
                                (lock-free)
```

## License

MIT License — see LICENSE file.
