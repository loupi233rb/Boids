# Boids

This repository contains a Boids (flocking) project. The primary implementation is in C++ (under `src/`) and uses the EasyX graphics library on Windows for rendering.

Note: A small Python Snake demo that was previously present has been removed.

## Project Structure

- `src/` - C++ source files (Boids simulation and supporting code).
- `include/` - C++ header files.
- `bin/` - Binaries and config files (project may write runtime output here).
- `requirements.txt` - (was used for Python demo) can be ignored for C++ build.

## Build (Windows)

This repo includes a VS Code `cppbuild` task (see `.vscode/tasks.json` if present).
You can build using that task from the VS Code Run/Build UI, or run the following command in PowerShell (adjust include/lib paths as necessary):

```powershell
# from repository root (example using the provided task args)
C:\TDM-GCC-64\bin\g++.exe -g -I${PWD}\include -I F:/environment/easyX/include -L F:/environment/easyX/lib src/*.cpp -o bin/main.exe -leasyX -lwinmm
```

If you are using the provided VS Code task, open the Command Palette and run `Tasks: Run Build Task` or press the configured build shortcut.

## Run

After building, run the executable produced in `bin/main.exe`:

```powershell
# Run built binary
.\bin\main.exe
```

## Dependencies

- EasyX (graphics library) — Windows. Install according to EasyX instructions and ensure the include/lib paths in the build task or command are correct.
- TDM-GCC or another MinGW/GCC toolchain on Windows for compiling.

## Development notes

- The prior Python Pygame Snake demo was used for quick prototyping and has been removed; if you need that demo back, check repository history (git) or ask to restore it.
- Consider adding a `.gitignore` to exclude build artifacts, `__pycache__/`, and virtual environments.

## Contributing

Pull requests are welcome. For changes that affect build paths or third-party libraries, please include updated build instructions in this `README.md`.
