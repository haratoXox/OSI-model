# OSI Layer Simulation in C++

A Windows C++ learning project that demonstrates how one application message can be wrapped and unwrapped as it travels through a simplified OSI-style protocol stack.

The project uses local TCP sockets for communication and adds its own small, educational headers at each simulated layer. It is a learning model, not an implementation of real Ethernet, IP, TCP, TLS, or DNS protocols.

## What it demonstrates

- A TCP client and server communicating on `127.0.0.1:9000`
- Length-prefixed frames at the simulated physical layer
- A payload length and checksum at the data-link layer
- Source address, destination address, and TTL fields at the network layer
- Source and destination ports at the transport layer
- A session identifier at the session layer
- XOR encoding/decoding at the presentation layer
- Message send/receive operations at the application layer

## Layer flow

```text
Application       message
    |
Presentation      XOR encodes the message
    |
Session           prepends a session ID
    |
Transport         prepends source and destination ports
    |
Network           prepends source/destination addresses and TTL
    |
Data Link         prepends payload length and checksum
    |
Physical          sends a length-prefixed byte frame over TCP
```

At the receiving end, the same layers run in reverse order to recover the original application message.

## Project structure

```text
.
+-- myserver.cpp    # Starts the local TCP server and sends/receives a message
+-- myclient.cpp    # Connects to the server and sends/receives a message
+-- myOSI.h         # Current OSI simulation implementation
+-- MY_OSI.cpp      # Alternate/copy of the simulation implementation
+-- osi_bytes.cpp   # Early byte-level experiment
+-- chatserver.cpp  # Reserved chat-server experiment
+-- chatclient.cpp  # Reserved chat-client experiment
`-- test.cpp        # Reserved test file
```

## Requirements

- Windows
- A C++17 compiler
- Winsock 2 (included with Windows / Visual Studio C++ tools)

## Build

Open a **Developer PowerShell for Visual Studio** in this directory and build the server and client separately:

```powershell
cl /EHsc /std:c++17 myserver.cpp /Fe:myserver.exe /link ws2_32.lib
cl /EHsc /std:c++17 myclient.cpp /Fe:myclient.exe /link ws2_32.lib
```

The implementation is currently included directly by `myOSI.h`, so do not also compile `MY_OSI.cpp` in the same command; doing so would duplicate function definitions.

> **Current work-in-progress note:** Before the first build, remove or comment the stray explanatory text immediately after the `#pragma comment(lib, "ws2_32.lib")` line in `myOSI.h`. That text is not valid C++ and prevents compilation.

## Run

Start the server first:

```powershell
.\myserver.exe
```

Then, in a second terminal:

```powershell
.\myclient.exe
```

Both programs use loopback (`127.0.0.1`) and TCP port `9000`, so no internet connection is required.

## Packet format used by this simulation

```text
[frame length: 4 bytes]
    [payload length: 1 byte]
    [checksum: 1 byte]
    [source address: 1 byte]
    [destination address: 1 byte]
    [TTL: 1 byte]
    [source port: 2 bytes]
    [destination port: 2 bytes]
    [session ID: 1 byte]
    [XOR-encoded message: variable length]
```

Fields are added while sending and removed in the reverse order while receiving.

## Learning notes

- The outer TCP socket provides the actual reliable transport between the two programs.
- The transport and network headers created by this project are **simulated headers** for learning; they are not the headers used by Windows networking.
- The checksum is a simple byte-sum modulo 256, useful for demonstrating integrity checks but not suitable for production networking.
- XOR encoding illustrates reversible transformation; it is not secure encryption.

## Suggested next improvements

1. Move declarations into `myOSI.h` and function definitions into `myOSI.cpp`.
2. Add error handling for `socket`, `bind`, `listen`, `connect`, `send`, and `recv`.
3. Implement `sendAll` and `recvAll`, because TCP may send or receive fewer bytes than requested.
4. Validate every received length before reading a frame.
5. Add a CMake build file and automated tests for each layer.
6. Extend the project with a DNS client that reuses the byte-parsing skills practiced here.

## License

This project does not currently include a license. Add one before accepting outside contributions or distributing the project under defined terms.
