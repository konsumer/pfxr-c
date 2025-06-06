# PFXR C Library

A C port of the [PFXR](https://github.com/achtaitaipai/pfxr) sound effects library that generates retro-style sound effects and outputs them as WAV files, in C. Use [this UI](https://achtaitaipai.github.io/pfxr/) to build your sound-effects.

## Overview

This library allows you to generate classic 8-bit/16-bit style sound effects programmatically using simple parameters or predefined templates. It's perfect for game development, audio tools, or any application that needs procedurally generated sound effects.

The C port maintains full compatibility with the original TypeScript/JavaScript version's templates and randomization algorithms, ensuring identical sound generation given the same parameters.

## Features

- **Template-based sound generation**: 10 predefined sound templates (laser, pickup, jump, explosion, etc.)
- **Custom sound configuration**: Full control over all synthesis parameters
- **WAV file output**: Generate standard 16-bit mono WAV files at 44.1kHz
- **Memory-based generation**: Get WAV data in memory without writing to disk
- **Deterministic randomization**: Same seed produces identical sounds
- **Cross-platform**: Pure C99 with minimal dependencies

## Quick Start

### Building

```bash
cd pfxr-c
make all
```

This creates:
- `build/libpfxr.a` - Static library
- `build/libpfxr.so` - Shared library
- `build/simple_example` - Basic example program
- `build/template_test` - Comprehensive test program

### Basic Usage

```c
#include "pfxr.h"

int main() {
    // Generate a laser sound with seed 999 and save to file
    int result = pfxr_create_sound_from_template_to_file(PFXR_TEMPLATE_LASER, 999, "laser.wav");
    if (result == 0) {
        printf("Laser sound created successfully!\n");
    }

    return 0;
}
```

### Memory-based Generation

```c
#include "pfxr.h"

int main() {
    // Generate sound in memory
    char* wav_data = pfxr_create_sound_from_template(PFXR_TEMPLATE_PICKUP, 12345);
    if (wav_data) {
        // wav_data now contains complete WAV file data
        // You can write it to a file, stream it, etc.

        // Don't forget to free the memory
        pfxr_free_wav_data(wav_data);
    }

    return 0;
}
```

## API Reference

### Core Functions

```c
// Generate sound from template and save to file
int pfxr_create_sound_from_template_to_file(pfxr_template_t template, int seed, const char* filename);

// Generate sound from template and return WAV data in memory
char* pfxr_create_sound_from_template(pfxr_template_t template, int seed);

// Generate sound from custom configuration and save to file
int pfxr_create_sound_from_config_to_file(const pfxr_sound_t* config, const char* filename);

// Generate sound from custom configuration and return WAV data in memory
char* pfxr_create_sound_from_config(const pfxr_sound_t* config);

// Free WAV data returned by memory-based functions
void pfxr_free_wav_data(char* wav_data);
```

### Templates

The library includes the following predefined templates:

- `PFXR_TEMPLATE_LASER` - Sci-fi laser/zap sounds
- `PFXR_TEMPLATE_PICKUP` - Item collection sounds
- `PFXR_TEMPLATE_JUMP` - Character jump sounds
- `PFXR_TEMPLATE_FALL` - Falling/descending sounds
- `PFXR_TEMPLATE_POWERUP` - Power-up/level-up sounds
- `PFXR_TEMPLATE_EXPLOSION` - Explosion/impact sounds
- `PFXR_TEMPLATE_BLIP` - Short beep/blip sounds
- `PFXR_TEMPLATE_HIT` - Attack/hit sounds
- `PFXR_TEMPLATE_FART` - Comedic sounds
- `PFXR_TEMPLATE_RANDOM` - Completely randomized parameters

### Custom Sound Configuration

For full control, create and modify a `pfxr_sound_t` structure:

```c
pfxr_sound_t config = pfxr_get_default_sound();

// Waveform (PFXR_WAVE_SINE, PFXR_WAVE_SAWTOOTH, PFXR_WAVE_SQUARE, PFXR_WAVE_TRIANGLE)
config.waveForm = PFXR_WAVE_SQUARE;
config.volume = 0.7f;

// Envelope
config.attackTime = 0.01f;    // Attack phase duration
config.sustainTime = 0.3f;    // Sustain phase duration
config.sustainPunch = 0.5f;   // Sustain punch amount
config.decayTime = 0.2f;      // Decay phase duration

// Pitch
config.frequency = 440.0f;    // Base frequency in Hz
config.pitchDelta = -200.0f;  // Pitch change amount
config.pitchDuration = 1.0f;  // Pitch change duration (0-1)
config.pitchDelay = 0.0f;     // Delay before pitch change starts

// Effects
config.vibratoRate = 5.0f;    // Vibrato frequency
config.vibratoDepth = 10.0f;  // Vibrato depth
config.tremoloRate = 0.0f;    // Tremolo frequency
config.tremoloDepth = 0.0f;   // Tremolo depth

// Filters
config.lowPassCutoff = 4000.0f;   // Low-pass filter cutoff
config.lowPassResonance = 0.0f;   // Low-pass filter resonance
config.highPassCutoff = 0.0f;     // High-pass filter cutoff
config.highPassResonance = 0.0f;  // High-pass filter resonance

// Phaser
config.phaserDepth = 0.0f;           // Phaser effect depth
config.phaserBaseFrequency = 100.0f; // Phaser base frequency
config.phaserLfoFrequency = 50.0f;   // Phaser LFO frequency

// Noise
config.noiseAmount = 0.0f;    // Amount of noise/distortion

// Generate the sound
pfxr_create_sound_from_config_to_file(&config, "custom_sound.wav");
```

## Examples

Run the included examples:

```bash
# Basic example - generates sounds from all templates
./build/simple_example

# Comprehensive test - shows template variations and custom configurations
./build/template_test
```

## Compatibility with Original PFXR

This C port maintains full compatibility with the original TypeScript PFXR library:

- **Same templates**: All templates produce identical sounds given the same seed
- **Same randomization**: Uses the same XorShift algorithm for deterministic results
- **Same parameters**: All sound parameters have identical ranges and behavior
- **Same audio processing**: Faithful reproduction of the synthesis pipeline

Example equivalency:

**TypeScript:**
```typescript
const sound = createSoundFromTemplate(TEMPLATES.LASER, 999)
```

**C:**
```c
char* wav_data = pfxr_create_sound_from_template(PFXR_TEMPLATE_LASER, 999);
```

Both will generate identical audio content.

## Building and Installation

### Requirements

- C99-compatible compiler (GCC, Clang, MSVC)
- Make (for build system)
- Math library (libm on Unix systems)

### Build Options

```bash
make all      # Build everything
make static   # Build static library only
make shared   # Build shared library only
make examples # Build example programs only
make debug    # Build with debug symbols
make clean    # Clean build files
```

### Installation

```bash
make install  # Install to /usr/local (requires sudo)
```

### Integration

**Static linking:**
```bash
gcc myprogram.c -L./build -lpfxr -lm -o myprogram
```

**Dynamic linking:**
```bash
gcc myprogram.c -L./build -lpfxr -lm -o myprogram
export LD_LIBRARY_PATH=./build:$LD_LIBRARY_PATH
```

## Technical Details

- **Sample Rate**: 44.1 kHz
- **Bit Depth**: 16-bit signed integers
- **Channels**: Mono
- **Output Format**: Standard WAV (RIFF) files
- **Maximum Duration**: 4 seconds per sound
- **Memory Usage**: ~700KB maximum per sound generation

## License

This C port maintains the same licensing as the original PFXR library. Please refer to the original project for licensing details.

## Contributing

Contributions are welcome! Please ensure:
- Code follows C99 standards
- All examples continue to work
- Memory leaks are avoided
- Cross-platform compatibility is maintained

## See Also

- [Original PFXR (TypeScript)](https://github.com/SilentImp/pfxr)
- [JSFXR (JavaScript)](https://github.com/chr15m/jsfxr)
- [SFXR (Original C++)](http://www.drpetter.se/project_sfxr.html)
