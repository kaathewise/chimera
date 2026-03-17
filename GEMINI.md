# Chimera Project Overview

Chimera is a sophisticated synthesizer firmware for the **Daisy Seed** (STM32H750) platform. It integrates high-quality DSP engines from Mutable Instruments (Plaits, Marbles) into a custom control framework featuring touch pads and knobs.

## Architecture

The project is organized into several sub-modules, each handling a specific part of the system:

- **`chimera/`**: The main application entry point and orchestration layer. The `Patch` class manages the high-level logic, switching between sequencer and voice control modes.
- **`voice/`**: Sound generation logic. It wraps the `plaits::ParticleEngine` (from Mutable Instruments Plaits) and adds a `ChannelPostProcessor` with Low Pass Gate (LPG) and Limiter.
- **`sequencer/`**: Generative sequencing logic. It uses the `TGenerator` and `RandomStream` from Mutable Instruments Marbles to create rhythmic patterns.
- **`simpletouch/`**: Hardware abstraction layer for inputs.
    - `Pads`: Handles MPR121 capacitive touch sensor.
    - `Knobs`: Manages ADC-connected potentiometers.
    - `Switches`: Handles digital switches.
- **`common/`**: Shared utilities, notably `ControlValue` for implementing "soft-takeover" (catch-up) logic when switching knob assignments.

External libraries that are not part of the project itself:

- **`eurorack/`**: A collection of DSP and utility code from Mutable Instruments (Plaits, Marbles, stmlib).
- **`libDaisy` & `DaisySP`**: Core libraries for the Daisy hardware and audio DSP primitives.

## Building and Running

The project uses a standard Makefile-based build system provided by libDaisy, with a custom `include.mk` to handle directory-preserving object mapping.

### Key Commands
- **Build a module**:
  ```bash
  make -C chimera
  ```

### Testing

It is not possible to automatically test the modules, apart from building the module, so extra care needs to be taken to ensure code quality.

*Note: Some modules are configured to run from internal SRAM (`APP_TYPE = BOOT_SRAM`) due to the size of lookup tables.*

## Development Conventions

- **C++ Standard**: C++20.
- **Style Guide**: Use Google C++ Style Guide. Use `cpplint --recursive --exclude=third_party .` to run linter.
- **Header includes**: Use absolute path from workspace root, with an exception for <daisy_seed.h>.
- **DSP Processing**: Audio is processed in blocks (default size 4 samples at 48kHz). DSP logic is generally encapsulated in classes with `Init()` and `Process()` methods.
- **Soft Takeover**: When adding new knob-controlled parameters, use `simpletouch::ControlValue` to ensure smooth transitions when switching control pages.
- **Hardware Integration**: The `simpletouch::Touch` class is the central point for accessing hardware inputs. Avoid direct hardware calls in DSP or high-level logic.

## Key Files
- `chimera/chimera.cpp`: Main entry point and audio callback.
- `chimera/patch.cpp`: High-level patch logic and control routing.
- `voice/voice.cpp`: Main synthesis voice implementation.
- `sequencer/sequencer.cpp`: Generative sequencing logic.
- `include.mk`: Custom build rules for directory-preserving object mapping.

@.gemini/rules.md