#ifndef PFXR_H
#define PFXR_H

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

// Audio constants
#define PFXR_SAMPLE_RATE 44100
#define PFXR_MAX_DURATION 4.0f
#define PFXR_MAX_SAMPLES (int)(PFXR_SAMPLE_RATE * PFXR_MAX_DURATION)

// Wave form types
typedef enum {
    PFXR_WAVE_SINE = 0,
    PFXR_WAVE_SAWTOOTH = 1,
    PFXR_WAVE_SQUARE = 2,
    PFXR_WAVE_TRIANGLE = 3
} pfxr_wave_type_t;

// Sound template types
typedef enum {
    PFXR_TEMPLATE_DEFAULT = 0,
    PFXR_TEMPLATE_PICKUP,
    PFXR_TEMPLATE_LASER,
    PFXR_TEMPLATE_JUMP,
    PFXR_TEMPLATE_FALL,
    PFXR_TEMPLATE_POWERUP,
    PFXR_TEMPLATE_EXPLOSION,
    PFXR_TEMPLATE_BLIP,
    PFXR_TEMPLATE_HIT,
    PFXR_TEMPLATE_FART,
    PFXR_TEMPLATE_RANDOM
} pfxr_template_t;

// Sound configuration structure
typedef struct {
    // Waveform and volume
    int waveForm;
    float volume;
    
    // Sound envelope
    float attackTime;
    float sustainTime;
    float sustainPunch;
    float decayTime;
    
    // Pitch
    float frequency;
    float pitchDelta;
    float pitchDuration;
    float pitchDelay;
    
    // Vibrato
    float vibratoRate;
    float vibratoDepth;
    
    // Tremolo
    float tremoloRate;
    float tremoloDepth;
    
    // Filters
    float highPassCutoff;
    float highPassResonance;
    float lowPassCutoff;
    float lowPassResonance;
    
    // Phaser
    float phaserBaseFrequency;
    float phaserLfoFrequency;
    float phaserDepth;
    
    // Noise
    float noiseAmount;
} pfxr_sound_t;

// WAV file header structure
typedef struct {
    char riff[4];           // "RIFF"
    uint32_t chunk_size;    // File size - 8
    char wave[4];           // "WAVE"
    char fmt[4];            // "fmt "
    uint32_t fmt_size;      // 16 for PCM
    uint16_t audio_format;  // 1 for PCM
    uint16_t num_channels;  // 1 for mono
    uint32_t sample_rate;   // 44100
    uint32_t byte_rate;     // sample_rate * num_channels * bits_per_sample / 8
    uint16_t block_align;   // num_channels * bits_per_sample / 8
    uint16_t bits_per_sample; // 16
    char data[4];           // "data"
    uint32_t data_size;     // Number of bytes in data
} __attribute__((packed)) pfxr_wav_header_t;

// Audio buffer structure
typedef struct {
    float* samples;
    int sample_count;
    int capacity;
} pfxr_audio_buffer_t;

// Random number generator state
typedef struct {
    uint32_t seed;
    uint32_t x, y, z, w;
} pfxr_random_t;

// Main API functions
char* pfxr_create_sound_from_template(pfxr_template_t template, int seed);
int pfxr_create_sound_from_template_to_file(pfxr_template_t template, int seed, const char* filename);
char* pfxr_create_sound_from_config(const pfxr_sound_t* config);
int pfxr_create_sound_from_config_to_file(const pfxr_sound_t* config, const char* filename);

// Utility functions
pfxr_sound_t pfxr_get_default_sound(void);
pfxr_sound_t pfxr_apply_template(pfxr_template_t template, int seed);
void pfxr_free_wav_data(char* wav_data);

// Random number generator functions
void pfxr_random_init(pfxr_random_t* rng, uint32_t seed);
float pfxr_random_float(pfxr_random_t* rng, float min, float max);
int pfxr_random_bool(pfxr_random_t* rng, float true_probability);
int pfxr_random_choice(pfxr_random_t* rng, const int* choices, int count);

// Audio processing functions
pfxr_audio_buffer_t* pfxr_create_audio_buffer(int capacity);
void pfxr_free_audio_buffer(pfxr_audio_buffer_t* buffer);
void pfxr_generate_sound(const pfxr_sound_t* config, pfxr_audio_buffer_t* buffer);

// WAV file functions
char* pfxr_create_wav_data(const float* samples, int sample_count, int* wav_size);
int pfxr_write_wav_file(const char* filename, const float* samples, int sample_count);

#ifdef __cplusplus
}
#endif

#endif // PFXR_H