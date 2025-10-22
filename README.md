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

## Targets

The emulator core is always built as a static library for inclusion in additional executables. The additional executables can be configured as CMake options using the GUI on Windows, or using build options on macOS/Linux:

```sh
cmake -S . -B build -DBUILD_TESTS=off
```

### Unit Tests (`BUILD_TESTS`)

If the unit test suite should be built for compilation. It requires an additional dependency to be downloaded and depends on Python being installed on the system to run a code generator, which may be too much hassle if you don't intend to expand emulator functionality.

## Configuration

To facilitate easier use of the emulator and to tailor it to the various implementations of the CHIP-8 that were created using contemporary hardware, the project provides compiler options to modify the emulator's behavior, which can be configured using CMake using the GUI on Windows, or using build options on macOS/Linux:

```sh
cmake -S . -B build -DENABLE_LOGS=off
```

### `INSTRUCTIONS_PER_SECOND`

The number of instructions that the emulator should target for executing in a single second. Contemporary hardware varied in their abilities to run the interpreter, so different games were created with different assumptions to the capabilities of the hardware. This option essentially limits the clock speed of the emulator, and might require adjustment for individual games.

If not provided, defaults to `700`. The recommended value is between 500 and 700.

### `DEFAULT_FONT`

The font that the emulator should load into memory when the emulator is initialized. For examples of the different supported fonts, [see this GitHub issue](https://github.com/mattmikolay/chip-8/issues/3).

If not provided, defaults to `FONT_CHIP48`. Supports the following options:

- `FONT_CHIP48`
- `FONT_COSMACVIP`
- `FONT_DREAM6800`
- `FONT_ETI660`

### `ENABLE_LOGS`

If runtime error logging to `stderr` should be enabled. These logs mostly consist of warnings to the user about using undefined behavior, or misusing instructions. This is largely not necessary if a known good ROM is used.

If not provided, defaults to `OFF`.

### `LEGACY_OFFSET_JUMP_BEHAVIOR`

If the legacy (COSMAC VIP) jump with offset (`0xBXNN`) behavior should be used. If enabled, `PC` will be set to the value of `XNN + V0`. If disabled, `PC` will be set to the value of `XNN + VX`.

If not provided, defaults to `ON`.

### `LEGACY_MEMORY_BEHAVIOR`

If the legacy (COSMAC VIP) store and load memory (`0xFX55` / `0xFX65`) behavior should be used. If enabled, `I` will be incremented by 1 for each register that the action was performed on, pointing it to the next memory address after the last one used (`Inew = Iold + X + 1`). If disabled, `I` will not be modified while processing the instruction.

If not provided, defaults to `OFF`.

### `LEGACY_SHIFT_BEHAVIOR`

If the legacy (COSMAC VIP) shift (`0x8XY6` / `0x8XYE`) behavior should be used. If enabled, `VX` will be set to the value of `VY`, and then `VX` will be shifted appropriately. If disabled, `VX` will be shifted in place and `Y` will go unused.

If not provided, defaults to `ON`.

## Testing

The project utilizes the [Unity framework](https://github.com/ThrowTheSwitch/Unity) to provide unit testing capabilities. Due to being entirely self-sufficient, the test suite is compiled into a single executable using test groups from the [Fixtures add-on](https://github.com/ThrowTheSwitch/Unity/tree/master/extras/fixture). A custom code generator is included for generating the test runners using this approach.

To run the code generator and compile the test executable, the `BUILD_TESTS` option must be enabled in CMake.

1. Write a test group, based on [Unity's second example](https://github.com/ThrowTheSwitch/Unity/tree/v2.6.1/examples/example_2) or any of the existing test suites in the project;
2. Generate the runner for the test group and update `main` for the test suite:

```sh
python tools/generate_unity_runners.py
```

3. Compile the test runners
4. Run the test executable at `build/bin/chip8_tests`
