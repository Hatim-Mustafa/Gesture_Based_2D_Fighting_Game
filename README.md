# Gesture Based Fighting Game

A small, gesture-driven 2D fighting game written in C++ using SFML. The project demonstrates gesture recognition for player input, sprite-based characters, simple networking for game-state transmission, and a Visual Studio solution for building on Windows.

## Key features

- **Gesture input:** control characters using mouse/gesture movements and keyboard fallbacks.
- **Sprite-based characters:** art and animations live under the `Assets` folder.
- **Local networking demo:** lightweight game-state sender and socket server (`game_state_sender.cpp`, `socket_server.cpp`) to demonstrate state synchronization and testing.
- **SFML rendering & audio:** uses SFML for windowing, graphics, input, and sound.
- **Visual Studio solution:** ready-to-open `Gesture_Based_Fighting_Game.sln` for development on Windows.

## Repository layout

- `main.cpp` — application entry point and game loop.
- `game_state_sender.cpp` — demo client that sends game state over the network.
- `socket_server.cpp` — simple UDP/TCP server for receiving game-state messages.
- `Assets/` — art, backgrounds, and sprites used by the game.
- `Gesture_Based_Fighting_Game.sln` / `.vcxproj` — Visual Studio project and solution files.

## Prerequisites

- Windows 10/11
- Visual Studio 2022 or newer with C++ workload
- SFML 2.6 (or compatible 2.x) installed locally

The Visual Studio project uses a local `SFML_DIR` CMake / property path rather than hardcoded system paths. Configure `SFML_DIR` to point to your SFML root folder before building, for example:

```
C:\SFML-2.6.2\
```

## Build (Visual Studio)

1. Open `Gesture_Based_Fighting_Game.sln` in Visual Studio.
2. Ensure `SFML_DIR` (Project > Properties > C/C++ > Additional Include Directories / Linker > Additional Library Directories) is set to your SFML installation directory.
3. Build the solution (Debug / Release as desired).

Notes:
- Do not commit machine-specific files such as `*.user`.
- If you prefer a command-line build, run the appropriate MSBuild command for your Visual Studio environment.

## Run

- Run the built executable from Visual Studio or from the `Debug`/`Release` output folder.
- If you want to test networking features, start `socket_server` first (or run the server binary), then run `game_state_sender` to stream demo state.

Example local test sequence:

```
# Start the server (in one terminal)
Gesture_Based_Fighting_Game_server.exe

# Start the client/sender (in another terminal)
Gesture_Based_Fighting_Game_sender.exe
```

Adjust executable names/path based on your build configuration and project settings.

## Controls

- Primary controls are gesture/mouse-based (see in-game help or `main.cpp` for recognition configuration).
- Keyboard fallbacks: arrow keys for movement, `Z`/`X`/`C` (or similar) for attacks — consult the source for the exact mapping.

## Assets & Licensing

The `Assets` directory contains subfolders for backgrounds and character sprites. Some third-party assets include `License.txt` files; review them before redistribution.

## Contributing

- Fork the repository, make changes on a branch, and open a PR with a clear description.
- Keep Visual Studio user files local and do not include large binary assets in PRs unless necessary.

## Troubleshooting

- If SFML symbols are missing at link time, verify `SFML_DIR` and library directories are correct and that runtime DLLs are available on `PATH` or next to the executable.
- For networking issues, ensure firewalls allow the chosen ports and that server/client IPs match your network configuration.

## Contact / Notes

This repository is intended as an educational demo of gesture-driven controls combined with simple networking. If you want, I can add a short development roadmap, build scripts, or CI integration — tell me which you'd like next.