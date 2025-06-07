# PFXR C Library - Single Header Edition

A C port of the [PFXR](https://github.com/achtaitaipai/pfxr) sound effects library that generates retro-style sound effects and outputs them as WAV files. This is a **single-header library** for easy integration into any C project.

## Overview

This library allows you to generate classic 8-bit/16-bit style sound effects programmatically using simple parameters or predefined templates. It's perfect for game development, audio tools, or any application that needs procedurally generated sound effects.

The C port maintains full compatibility with the original TypeScript/JavaScript version's templates and randomization algorithms, ensuring identical sound generation given the same parameters.

## Single-Header Library

### Quick Integration

1. **Copy `pfxr.h` to your project**
2. **In ONE source file, define the implementation:**
   ```c
   #define PFXR_IMPLEMENTATION
   #include "pfxr.h"
   ```
3. **In other files, just include the header:**
   ```c
   #include "pfxr.h"
   ```
4. **Compile with math library:**
   ```bash
   gcc -o myapp myapp.c -lm
   ```

## Features

- **Single-header design**: Drop one file into your project and go
- **Template-based sound generation**: 10 predefined sound templates (laser, pickup, jump, explosion, etc.)
- **Custom sound configuration**: Full control over all synthesis parameters
- **WAV file output**: Generate standard 16-bit mono WAV files at 44.1kHz
- **Memory-based generation**: Get WAV data in memory without writing to disk
- **Deterministic randomization**: Same seed produces identical sounds
- **Cross-platform**: Pure C99 with minimal dependencies
- **URL compatibility**: Use [the PFXR web UI](https://achtaitaipai.github.io/pfxr/) to design sounds

## Quick Start

### Minimal Example

```c
#define PFXR_IMPLEMENTATION
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
#define PFXR_IMPLEMENTATION
#include "pfxr.h"

int main() {
    // Generate sound in memory
    char* wav_data = pfxr_create_sound_from_template(PFXR_TEMPLATE_PICKUP, 12345);
    if (wav_data) {
        // wav_data now contains complete WAV file data
        // You can write it to a file, stream it, etc.

        // Example: Save to file manually
        FILE* file = fopen("pickup.wav", "wb");
        pfxr_wav_header_t* header = (pfxr_wav_header_t*)wav_data;
        fwrite(wav_data, 1, header->chunk_size + 8, file);
        fclose(file);

        // Don't forget to free the memory
        pfxr_free_wav_data(wav_data);
    }
    return 0;
}
```

### All Available Templates

```c
#define PFXR_IMPLEMENTATION
#include "pfxr.h"

int main() {
    // Generate sounds from all templates with seed 999
    pfxr_create_sound_from_template_to_file(PFXR_TEMPLATE_LASER, 999, "laser.wav");
    pfxr_create_sound_from_template_to_file(PFXR_TEMPLATE_PICKUP, 999, "pickup.wav");
    pfxr_create_sound_from_template_to_file(PFXR_TEMPLATE_JUMP, 999, "jump.wav");
    pfxr_create_sound_from_template_to_file(PFXR_TEMPLATE_FALL, 999, "fall.wav");
    pfxr_create_sound_from_template_to_file(PFXR_TEMPLATE_POWERUP, 999, "powerup.wav");
    pfxr_create_sound_from_template_to_file(PFXR_TEMPLATE_EXPLOSION, 999, "explosion.wav");
    pfxr_create_sound_from_template_to_file(PFXR_TEMPLATE_BLIP, 999, "blip.wav");
    pfxr_create_sound_from_template_to_file(PFXR_TEMPLATE_HIT, 999, "hit.wav");
    pfxr_create_sound_from_template_to_file(PFXR_TEMPLATE_FART, 999, "fart.wav");

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

// Free sound configuration
void pfxr_free_sound_config(pfxr_sound_t* config);
```

### URL Functions

```c
// Generate sound configuration from PFXR URL (compatible with web UI)
pfxr_sound_t* pfxr_create_params_from_url(const char* url);

// Generate URL from sound configuration
char* pfxr_get_url_from_params(const pfxr_sound_t* config);
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
#define PFXR_IMPLEMENTATION
#include "pfxr.h"

int main() {
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
    config.frequency = 440.0f;    // Base frequency in Hz (A4 note)
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

    return 0;
}
```

## Building Examples

The repository includes example programs to demonstrate usage:

```bash
# Using the simplified Makefile for single-header
make -f Makefile.single-header examples

# Or compile examples manually
gcc -o simple_example examples/simple_example.c -lm
gcc -o api_demo examples/api_demo.c -lm
gcc -o template_test examples/template_test.c -lm
```

## Compatibility with Original PFXR

This C port maintains full compatibility with the original TypeScript PFXR library:

- **Same templates**: All templates produce identical sounds given the same seed
- **Same randomization**: Uses the same XorShift algorithm for deterministic results
- **Same parameters**: All sound parameters have identical ranges and behavior
- **Same audio processing**: Faithful reproduction of the synthesis pipeline
- **URL compatibility**: You can use [the PFXR web UI](https://achtaitaipai.github.io/pfxr/) to design sounds, copy the URL, and use it in your C code

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

### Using URLs from Web UI

```c
#define PFXR_IMPLEMENTATION
#include "pfxr.h"

int main() {
    // URL copied from https://achtaitaipai.github.io/pfxr/
    const char* url = "https://achtaitaipai.github.io/pfxr/?fx=2,0.5,0,0.3,0,0.2,440,0,1,0,10,20,0,0,0,0,4000,0,100,50,0,0";

    pfxr_sound_t* sound = pfxr_create_params_from_url(url);
    if (sound) {
        pfxr_create_sound_from_config_to_file(sound, "from_web_ui.wav");
        pfxr_free_sound_config(sound);
    }

    return 0;
}
```

## Single-Header Benefits

- **Zero build complexity**: Just drop `pfxr.h` into your project
- **No external dependencies**: Everything is self-contained
- **Easy distribution**: Share one file instead of entire libraries
- **Simple integration**: Works with any build system or no build system
- **Portable**: Works on any platform with a C99 compiler

## Technical Details

- **Sample Rate**: 44.1 kHz
- **Bit Depth**: 16-bit signed integers
- **Channels**: Mono
- **Output Format**: Standard WAV (RIFF) files
- **Maximum Duration**: 4 seconds per sound
- **Memory Usage**: ~700KB maximum per sound generation
- **Dependencies**: Only standard C library and math library (libm)

## Memory Management

Always free memory returned by the library:

```c
// Free WAV data
char* wav_data = pfxr_create_sound_from_template(PFXR_TEMPLATE_LASER, 999);
if (wav_data) {
    // Use wav_data...
    pfxr_free_wav_data(wav_data);  // Always free!
}

// Free sound configurations
pfxr_sound_t* sound = pfxr_create_params_from_url(url);
if (sound) {
    // Use sound...
    pfxr_free_sound_config(sound);  // Always free!
}

// Free URL strings
char* url = pfxr_get_url_from_params(&config);
if (url) {
    // Use url...
    free(url);  // Standard free() for URLs
}
```

## Platform Support

The single-header library works on:

- **Linux** (GCC, Clang)
- **macOS** (Clang, GCC)
- **Windows** (MSVC, MinGW, Clang)
- **Embedded systems** (with C99 support)
- **WebAssembly** (with Emscripten)

## Common Usage Patterns

### Game Development

```c
// In your game initialization
#define PFXR_IMPLEMENTATION
#include "pfxr.h"

// Generate sounds on-demand
void play_laser_sound() {
    char* wav_data = pfxr_create_sound_from_template(PFXR_TEMPLATE_LASER, rand());
    if (wav_data) {
        // Stream to audio system, save temporarily, etc.
        pfxr_free_wav_data(wav_data);
    }
}
```

### Procedural Audio

```c
// Generate different variations of the same sound type
void generate_explosion_variations() {
    for (int i = 0; i < 10; i++) {
        char filename[64];
        snprintf(filename, sizeof(filename), "explosion_%d.wav", i);
        pfxr_create_sound_from_template_to_file(PFXR_TEMPLATE_EXPLOSION, i, filename);
    }
}
```

## License

This C port maintains the same licensing as the [original PFXR library](https://github.com/achtaitaipai/pfxr). Please refer to the original project for licensing details.


## See Also

- [Original PFXR (TypeScript)](https://github.com/achtaitaipai/pfxr)
- [PFXR Web UI](https://achtaitaipai.github.io/pfxr/) - Design sounds visually
- [JSFXR (JavaScript)](https://github.com/chr15m/jsfxr)
- [SFXR (Original C++)](http://www.drpetter.se/project_sfxr.html)
