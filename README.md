# Secure Multiplayer Game Server

This project is a real-time multiplayer game server written in C. It uses TCP sockets and will eventually include SDL2 graphics, server-side input validation, and basic cheat detection.  
The server supports multiple players and is designed to be secure, responsive, and cross-platform.

## Week 1 Features
- Server creates a TCP socket
- Binds to `127.0.0.1:8080`
- Accepts one client
- Echoes all input back to client

## Week 2 Goals
- Set up hosting multiple clients using threads
- Ensure clean connection and disconnection of clients

## Installation
```bash
git clone https://github.com/carbon1423/secure-multiplayer-game-server.git
cd server
make
./server
```
```bash
# In another terminal
telnet 127.0.0.1 8080
```
## Optional Testing Tools

You can use `telnet` to manually test the server during development.

### Install on Debian/Ubuntu:
```bash
sudo apt install telnet


