# real-life-sim

Small C++/OpenGL prototype engine.

## Current prototype

The current `main.cpp` entry point is a first-person engine shell built on the previous animation loader/state machine. The local player body and animation rig are intentionally not rendered from the first-person camera, while animation state continues to update internally for later reuse by third-person views, mirrors, or other characters.

## Controls

- `W`, `A`, `S`, `D` — move relative to the current camera yaw.
- Mouse — look around in first person.
- `Page Up` / `Page Down` — switch vertical map levels from the previous prototype.
- `Esc` — close the window.
