# Secure Multiplayer Game Server

This project is a real-time multiplayer game server written in C. It uses TCP sockets and will eventually include SDL2 graphics, server-side input validation, and basic cheat detection.  
The server supports multiple players and is designed to be secure, responsive, and cross-platform.

## Current Features
- Server creates a TCP socket
- Binds to `127.0.0.1:8080`
- Accepts one client
- Set up hosting multiple clients using threads
- Clean connection and disconnection of clients
- Basic SDL2 game logic
- Have the ability to display multiple clients at once, but randomly crashes
- `test_client.c` creates a non graphical client that "moves" every second
- Update each clients screens with the other players positions
- Broadcaster stops sending signals if a client is no longer active
- Player Gravity
- Player horizontal accel and friction
- Server side inputs
- Players are able to shoot projectiles 
- Player Health
- Platforms and tilemap

## Goals
- Implement falling through a platform
- Implement walls

## Issues
- Player when holding jump flies through the platform
- When a player falls off the map it stops anyone from being able to join



## Installation/Testing
```bash
git clone https://github.com/carbon1423/secure-multiplayer-game-server.git
cd server
make
./server
```
```bash
# In another terminal
cd client
make
./client
```
This should now show you a basic screen and when you use `wasd` it will move, the server client will then print out the position values of the square.


