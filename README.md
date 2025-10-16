# CHIP-8

A [CHIP-8](https://en.wikipedia.org/wiki/CHIP-8) interpreter written with [Raylib](https://www.raylib.com/) in C.

## Setup

The project is created using CMake for cross-platform compatibility. On Windows, the preferred method of building the project is using Visual Studio's CMake integration. On macOS and Linux, a small Makefile wrapper has been created for generating the builds:

1. Build and compile the project:

```sh
make build
```

2. Run the executable:

```sh
make run
```

## Testing

The project is configured with unit tests using the [Unity framework](https://github.com/ThrowTheSwitch/Unity). Due to being entirely self-sufficient, the test suite is compiled into a single executable using test groups from the [Fixtures add-on](https://github.com/ThrowTheSwitch/Unity/tree/master/extras/fixture). A custom code generator is included for generating the test runners using this approach.

1. Write a test group, based on [Unity's second example](https://github.com/ThrowTheSwitch/Unity/tree/v2.6.1/examples/example_2) or any of the existing test suites in the project;
2. Generate the runner for the test group and update `main` for the test suite:

```sh
python tools/generate_unity_runners.py
```

3. Compile the test runners
4. Run the test executable at `build/bin/chip8_tests`
