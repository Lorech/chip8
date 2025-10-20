# CHIP-8

A [CHIP-8](https://en.wikipedia.org/wiki/CHIP-8) interpreter written with [Raylib](https://www.raylib.com/) in C.

## Setup

The project is created using CMake for cross-platform compatibility.

### Windows

Use Visual Studio's CMake integration to open and run the project. This is currently untested.

### macOS / Linux

Use CMake directly:

1. Build the project:

```sh
cmake -S . -B build
```

2. Compile the executable:

```sh
cmake --build build
```

3. Run the emulator:

```sh
./build/bin/chip8 roms/IBM\ Logo.ch8
```

## Testing

The project utilizes the [Unity framework](https://github.com/ThrowTheSwitch/Unity). to provide unit testing capabilities. Due to being entirely self-sufficient, the test suite is compiled into a single executable using test groups from the [Fixtures add-on](https://github.com/ThrowTheSwitch/Unity/tree/master/extras/fixture). A custom code generator is included for generating the test runners using this approach.

1. Write a test group, based on [Unity's second example](https://github.com/ThrowTheSwitch/Unity/tree/v2.6.1/examples/example_2) or any of the existing test suites in the project;
2. Generate the runner for the test group and update `main` for the test suite:

```sh
python tools/generate_unity_runners.py
```

3. Compile the test runners
4. Run the test executable at `build/bin/chip8_tests`
