# Chimera Project Overview

This project is a C++ audio synthesis environment built for the **Daisy Audio Platform**. It combines standard Daisy libraries with custom hardware abstractions and synthesis modules, including ports from Mutable Instruments' Eurorack modules.

## Project Structure

- **libDaisy/**: Hardware Abstraction Library (HAL) for the Daisy platform. Provides access to audio, MIDI, USB, and peripheral drivers (SPI, I2C, etc.).
- **DaisySP/**: A powerful DSP library containing modular components for audio software (oscillators, filters, envelopes, etc.).
- **eurorack/**: Contains code from Mutable Instruments Eurorack modules.
    - `marbles/`: Implementation of the Marbles random sampler module.
- **sequencer/**: Custom sequencer implementation.
    - `t_generator.cpp/h`: Ported and modified gate generator from Marbles.
    - `controls.cpp/h`: Mapping of hardware inputs to sequencer parameters.
    - `sequencer_check.cpp`: Test utility for the sequencer module.
- **simpletouch/**: Hardware-specific interface library for a custom control surface.
    - `knobs.cpp/h`: Interface for 8 analog knobs.
    - `pads.h`: Interface for 12 touch pads (MPR121).
    - `switches.cpp/h`: Interface for switches.
    - `controls_check.cpp`: Test utility for hardware controls.

## Building and Running

The project uses `make` for building. Each main component directory (`DaisySP`, `libDaisy`, `sequencer`, `simpletouch`) contains its own `Makefile`.

### Building a module

To build a specific module (e.g., the sequencer test):

```bash
make -C sequencer
```

### Testing

It is not possible to automatically test the modules, apart from building the module, so extra care needs to be taken to ensure code quality.

*Note: Some modules like `sequencer` are configured to run from internal SRAM (`APP_TYPE = BOOT_SRAM`) due to the size of lookup tables.*

## Development Conventions

- **Language**: C++20.
- **Namespace**:
    - `sequencer`: For sequencer-specific logic.
    - `simpletouch`: For hardware interface logic.
    - `daisy`: For libDaisy core functionality.
    - `stmlib`: For low-level STM32 utility code (used in eurorack ports).
- **Header Guards**: Prefer `#ifndef #define` pattern over `#pragma once`.
- **Style Guide**: Follow Google C++ Style Guide.
- **Includes**: Use absolute paths from workspace root, except for <daisy_seed.h> and <daisysp.h>.
- **Initialization**: Hardware and module initialization typically happens in `main()` using `hw.Init()` and specific `.Init()` methods for each module.
- **Audio Processing**: High-priority processing (like knob updates and sequencer logic) should be handled in the `AudioCallback` provided to `hw.StartAudio()`.
- **Logging**: Use `DaisySeed::Print` and `DaisySeed::PrintLine` for serial debugging. Ensure `DaisySeed::StartLog()` is called.

## Key Files

- `sequencer/sequencer_check.cpp`: Best example of how the `sequencer` and `simpletouch` modules integrate.
- `simpletouch/touch.h`: Main entry point for hardware control access.
- `sequencer/t_generator.cpp`: Core logic for gate generation, including jitter and probability-based sequencing.

## Sync Policy

Before using any file-editing tool (write_file, replace), you MUST first use read_file on the target file to ensure your context matches the current disk state. Do not assume your cached memory of the file is up to date.