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

// URL functions
pfxr_sound_t* pfxr_create_params_from_url(const char* url);
char* pfxr_get_url_from_params(const pfxr_sound_t* config);
void pfxr_free_sound_config(pfxr_sound_t* config);

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

// ============================================================================
// IMPLEMENTATION
// ============================================================================

#ifdef PFXR_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <ctype.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// ============================================================================
// TEMPLATES IMPLEMENTATION
// ============================================================================

// Default sound configuration (matches TypeScript defaultSound)
pfxr_sound_t pfxr_get_default_sound(void) {
    pfxr_sound_t sound;
    
    // Waveform and volume
    sound.waveForm = 0;  // SINE
    sound.volume = 0.5f;
    
    // Sound envelope
    sound.attackTime = 0.0f;
    sound.sustainTime = 0.07f;
    sound.sustainPunch = 0.0f;
    sound.decayTime = 0.3f;
    
    // Pitch
    sound.frequency = 700.0f;
    sound.pitchDelta = 0.0f;
    sound.pitchDuration = 1.0f;
    sound.pitchDelay = 0.0f;
    
    // Vibrato
    sound.vibratoRate = 0.0f;
    sound.vibratoDepth = 0.0f;
    
    // Tremolo
    sound.tremoloRate = 0.0f;
    sound.tremoloDepth = 0.0f;
    
    // Filters
    sound.highPassCutoff = 0.0f;
    sound.highPassResonance = 0.0f;
    sound.lowPassCutoff = 4000.0f;
    sound.lowPassResonance = 0.0f;
    
    // Phaser
    sound.phaserBaseFrequency = 100.0f;
    sound.phaserLfoFrequency = 50.0f;
    sound.phaserDepth = 0.0f;
    
    // Noise
    sound.noiseAmount = 0.0f;
    
    return sound;
}

// Apply template to generate sound configuration
pfxr_sound_t pfxr_apply_template(pfxr_template_t template, int seed) {
    pfxr_sound_t sound = pfxr_get_default_sound();
    pfxr_random_t rng;
    pfxr_random_init(&rng, (uint32_t)seed);
    
    switch (template) {
        case PFXR_TEMPLATE_DEFAULT:
            // No changes from default
            break;
            
        case PFXR_TEMPLATE_PICKUP: {
            int wave_choices[] = {0, 1, 2, 3};
            sound.waveForm = pfxr_random_choice(&rng, wave_choices, 4);
            sound.sustainPunch = pfxr_random_float(&rng, 0.0f, 0.8f);
            sound.sustainTime = pfxr_random_float(&rng, 0.05f, 0.2f);
            sound.decayTime = pfxr_random_float(&rng, 0.1f, 0.3f);
            sound.frequency = pfxr_random_float(&rng, 900.0f, 1700.0f);
            
            if (pfxr_random_bool(&rng, 0.5f)) {
                sound.pitchDelta = pfxr_random_float(&rng, 100.0f, 500.0f);
                sound.pitchDuration = 0.0f;
                sound.pitchDelay = pfxr_random_float(&rng, 0.0f, 0.7f);
            }
            break;
        }
        
        case PFXR_TEMPLATE_LASER: {
            int wave_choices[] = {0, 1, 2, 3};
            sound.waveForm = pfxr_random_choice(&rng, wave_choices, 4);
            sound.sustainPunch = pfxr_random_float(&rng, 0.0f, 0.8f);
            sound.sustainTime = pfxr_random_float(&rng, 0.05f, 0.1f);
            sound.decayTime = pfxr_random_float(&rng, 0.0f, 0.2f);
            sound.frequency = pfxr_random_float(&rng, 100.0f, 1300.0f);
            sound.pitchDelta = pfxr_random_float(&rng, -sound.frequency, -100.0f);
            sound.pitchDuration = 1.0f;
            
            float delay_choices[] = {0.0f, pfxr_random_float(&rng, 0.0f, 0.3f)};
            sound.pitchDelay = delay_choices[pfxr_random_bool(&rng, 0.5f)];
            break;
        }
        
        case PFXR_TEMPLATE_JUMP: {
            int wave_choices[] = {1, 2};
            sound.waveForm = pfxr_random_choice(&rng, wave_choices, 2);
            sound.sustainPunch = pfxr_random_float(&rng, 0.0f, 0.8f);
            sound.sustainTime = pfxr_random_float(&rng, 0.2f, 0.5f);
            sound.decayTime = pfxr_random_float(&rng, 0.1f, 0.2f);
            sound.frequency = pfxr_random_float(&rng, 100.0f, 500.0f);
            sound.pitchDelta = pfxr_random_float(&rng, 200.0f, 500.0f);
            sound.pitchDuration = 1.0f;
            
            float delay_choices[] = {0.0f, pfxr_random_float(&rng, 0.0f, 0.3f)};
            sound.pitchDelay = delay_choices[pfxr_random_bool(&rng, 0.5f)];
            break;
        }
        
        case PFXR_TEMPLATE_FALL: {
            int wave_choices[] = {1, 2, 3};
            sound.waveForm = pfxr_random_choice(&rng, wave_choices, 3);
            sound.sustainPunch = 0.0f;
            sound.sustainTime = pfxr_random_float(&rng, 0.2f, 0.5f);
            sound.decayTime = pfxr_random_float(&rng, 0.2f, 0.5f);
            sound.frequency = pfxr_random_float(&rng, 80.0f, 500.0f);
            sound.pitchDelta = -sound.frequency;
            sound.pitchDuration = 1.0f;
            sound.pitchDelay = pfxr_random_float(&rng, 0.0f, 0.2f);
            sound.vibratoRate = pfxr_random_float(&rng, 8.0f, 18.0f);
            sound.vibratoDepth = pfxr_random_float(&rng, 10.0f, 30.0f);
            sound.tremoloRate = pfxr_random_float(&rng, 5.0f, 18.0f);
            sound.tremoloDepth = pfxr_random_float(&rng, 0.0f, 1.0f);
            break;
        }
        
        case PFXR_TEMPLATE_POWERUP: {
            int wave_choices[] = {0, 1, 2, 3};
            sound.waveForm = pfxr_random_choice(&rng, wave_choices, 4);
            sound.sustainPunch = pfxr_random_float(&rng, 0.0f, 1.0f);
            sound.sustainTime = pfxr_random_float(&rng, 0.2f, 0.5f);
            sound.decayTime = pfxr_random_float(&rng, 0.1f, 0.5f);
            sound.frequency = pfxr_random_float(&rng, 200.0f, 1000.0f);
            sound.pitchDelta = pfxr_random_float(&rng, 100.0f, 300.0f);
            sound.pitchDuration = 1.0f;
            
            float delay_choices[] = {0.0f, pfxr_random_float(&rng, 0.0f, 0.3f)};
            sound.pitchDelay = delay_choices[pfxr_random_bool(&rng, 0.5f)];
            sound.vibratoRate = pfxr_random_float(&rng, 10.0f, 18.0f);
            sound.vibratoDepth = pfxr_random_float(&rng, 50.0f, 100.0f);
            break;
        }
        
        case PFXR_TEMPLATE_EXPLOSION: {
            int wave_choices[] = {0, 1, 2, 3};
            sound.waveForm = pfxr_random_choice(&rng, wave_choices, 4);
            sound.volume = 0.3f;
            sound.sustainPunch = pfxr_random_float(&rng, 0.0f, 0.3f);
            sound.sustainTime = pfxr_random_float(&rng, 0.4f, 1.3f);
            sound.decayTime = pfxr_random_float(&rng, 0.1f, 0.5f);
            sound.frequency = pfxr_random_float(&rng, 0.0f, 200.0f);
            sound.pitchDelta = -sound.frequency;
            sound.pitchDuration = 1.0f;
            sound.pitchDelay = pfxr_random_float(&rng, 0.0f, 0.3f);
            sound.vibratoRate = pfxr_random_float(&rng, 0.0f, 70.0f);
            sound.vibratoDepth = pfxr_random_float(&rng, 0.0f, 100.0f);
            sound.tremoloRate = pfxr_random_float(&rng, 0.0f, 70.0f);
            sound.tremoloDepth = pfxr_random_float(&rng, 0.0f, 1.0f);
            sound.phaserDepth = pfxr_random_float(&rng, 300.0f, 1000.0f);
            sound.noiseAmount = pfxr_random_float(&rng, 300.0f, 500.0f);
            break;
        }
        
        case PFXR_TEMPLATE_BLIP: {
            int wave_choices[] = {0, 1, 2, 3};
            sound.waveForm = pfxr_random_choice(&rng, wave_choices, 4);
            sound.sustainTime = pfxr_random_float(&rng, 0.02f, 0.1f);
            sound.decayTime = pfxr_random_float(&rng, 0.0f, 0.04f);
            sound.frequency = pfxr_random_float(&rng, 600.0f, 3000.0f);
            break;
        }
        
        case PFXR_TEMPLATE_HIT: {
            int wave_choices[] = {0, 1, 2, 3};
            sound.waveForm = pfxr_random_choice(&rng, wave_choices, 4);
            sound.sustainTime = pfxr_random_float(&rng, 0.01f, 0.03f);
            sound.sustainPunch = pfxr_random_float(&rng, 0.0f, 0.5f);
            sound.decayTime = pfxr_random_float(&rng, 0.0f, 0.2f);
            sound.frequency = pfxr_random_float(&rng, 20.0f, 500.0f);
            sound.pitchDelta = pfxr_random_float(&rng, -sound.frequency, -sound.frequency * 0.2f);
            sound.noiseAmount = pfxr_random_float(&rng, 0.0f, 100.0f);
            break;
        }
        
        case PFXR_TEMPLATE_FART: {
            sound.waveForm = 1; // SAWTOOTH
            sound.volume = 0.7f;
            sound.sustainPunch = pfxr_random_float(&rng, 0.0f, 0.2f);
            sound.sustainTime = pfxr_random_float(&rng, 0.1f, 0.5f);
            sound.decayTime = pfxr_random_float(&rng, 0.3f, 0.5f);
            sound.frequency = pfxr_random_float(&rng, 30.0f, 150.0f);
            sound.pitchDelta = -sound.frequency / 2.0f;
            sound.pitchDuration = 1.0f;
            sound.pitchDelay = 0.1f;
            sound.vibratoRate = pfxr_random_float(&rng, 8.0f, 18.0f);
            sound.vibratoDepth = pfxr_random_float(&rng, 10.0f, 30.0f);
            sound.tremoloRate = pfxr_random_float(&rng, 35.0f, 70.0f);
            sound.tremoloDepth = pfxr_random_float(&rng, 0.6f, 1.0f);
            sound.lowPassCutoff = sound.frequency * 10.0f;
            sound.lowPassResonance = 10.0f;
            sound.noiseAmount = pfxr_random_float(&rng, 0.0f, 30.0f);
            break;
        }
        
        case PFXR_TEMPLATE_RANDOM: {
            // Generate completely random parameters within valid ranges
            int wave_choices[] = {0, 1, 2, 3};
            sound.waveForm = pfxr_random_choice(&rng, wave_choices, 4);
            sound.volume = pfxr_random_float(&rng, 0.0f, 1.0f);
            sound.attackTime = pfxr_random_float(&rng, 0.0f, 2.0f);
            sound.sustainTime = pfxr_random_float(&rng, 0.0f, 2.0f);
            sound.sustainPunch = pfxr_random_float(&rng, 0.0f, 1.0f);
            sound.decayTime = pfxr_random_float(&rng, 0.0f, 2.0f);
            sound.frequency = pfxr_random_float(&rng, 0.0f, 4000.0f);
            sound.pitchDelta = pfxr_random_float(&rng, -4000.0f, 4000.0f);
            sound.pitchDuration = pfxr_random_float(&rng, 0.0f, 1.0f);
            sound.pitchDelay = pfxr_random_float(&rng, 0.0f, 1.0f);
            sound.vibratoRate = pfxr_random_float(&rng, 0.0f, 70.0f);
            sound.vibratoDepth = pfxr_random_float(&rng, 0.0f, 100.0f);
            sound.tremoloRate = pfxr_random_float(&rng, 0.0f, 70.0f);
            sound.tremoloDepth = pfxr_random_float(&rng, 0.0f, 1.0f);
            sound.highPassCutoff = pfxr_random_float(&rng, 0.0f, 4000.0f);
            sound.highPassResonance = pfxr_random_float(&rng, 0.0f, 30.0f);
            sound.lowPassCutoff = pfxr_random_float(&rng, 0.0f, 4000.0f);
            sound.lowPassResonance = pfxr_random_float(&rng, 0.0f, 30.0f);
            sound.phaserBaseFrequency = pfxr_random_float(&rng, 0.0f, 1000.0f);
            sound.phaserLfoFrequency = pfxr_random_float(&rng, 0.0f, 200.0f);
            sound.phaserDepth = pfxr_random_float(&rng, 0.0f, 1000.0f);
            sound.noiseAmount = pfxr_random_float(&rng, 0.0f, 500.0f);
            break;
        }
    }
    
    return sound;
}

// ============================================================================
// RANDOM NUMBER GENERATOR IMPLEMENTATION
// ============================================================================

static uint32_t pfxr_random_uint32(pfxr_random_t* rng);

void pfxr_random_init(pfxr_random_t* rng, uint32_t seed) {
    if (seed == 0) {
        seed = (uint32_t)time(NULL);
    }
    
    rng->seed = seed;
    rng->x = seed & 0xffffffff;
    rng->y = 362436069;
    rng->z = 521288629;
    rng->w = 88675123;
    
    // Warm up the generator (same as TypeScript version)
    for (int i = 0; i < 32; i++) {
        pfxr_random_uint32(rng);
    }
}

static uint32_t pfxr_random_uint32(pfxr_random_t* rng) {
    uint32_t t = rng->x ^ ((rng->x << 11) & 0xffffffff);
    rng->x = rng->y;
    rng->y = rng->z;
    rng->z = rng->w;
    rng->w = rng->w ^ (rng->w >> 19) ^ (t ^ (t >> 8));
    return rng->w + 0x80000000;
}

float pfxr_random_float(pfxr_random_t* rng, float min, float max) {
    if (max < min) {
        float temp = min;
        min = max;
        max = temp;
    }
    
    uint32_t random_val = pfxr_random_uint32(rng);
    float normalized = (float)random_val / (float)0xffffffff;
    return min + (max - min) * normalized;
}

int pfxr_random_bool(pfxr_random_t* rng, float true_probability) {
    return pfxr_random_float(rng, 0.0f, 1.0f) < true_probability;
}

int pfxr_random_choice(pfxr_random_t* rng, const int* choices, int count) {
    if (count <= 0) return 0;
    int index = (int)pfxr_random_float(rng, 0.0f, (float)count);
    if (index >= count) index = count - 1;
    return choices[index];
}

// ============================================================================
// AUDIO BUFFER IMPLEMENTATION
// ============================================================================

// Create audio buffer with specified capacity
pfxr_audio_buffer_t* pfxr_create_audio_buffer(int capacity) {
    pfxr_audio_buffer_t* buffer = malloc(sizeof(pfxr_audio_buffer_t));
    if (!buffer) return NULL;
    
    buffer->samples = malloc(capacity * sizeof(float));
    if (!buffer->samples) {
        free(buffer);
        return NULL;
    }
    
    buffer->capacity = capacity;
    buffer->sample_count = 0;
    memset(buffer->samples, 0, capacity * sizeof(float));
    
    return buffer;
}

// Free audio buffer
void pfxr_free_audio_buffer(pfxr_audio_buffer_t* buffer) {
    if (buffer) {
        if (buffer->samples) {
            free(buffer->samples);
        }
        free(buffer);
    }
}

// ============================================================================
// AUDIO GENERATION IMPLEMENTATION
// ============================================================================

// Clamp value between min and max
static float clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

// Generate sine wave
static float generate_sine(float phase) {
    return sinf(phase * 2.0f * M_PI);
}

// Generate sawtooth wave
static float generate_sawtooth(float phase) {
    return 2.0f * (phase - floorf(phase + 0.5f));
}

// Generate square wave
static float generate_square(float phase) {
    return (phase - floorf(phase)) < 0.5f ? -1.0f : 1.0f;
}

// Generate triangle wave
static float generate_triangle(float phase) {
    float t = phase - floorf(phase);
    return t < 0.5f ? (4.0f * t - 1.0f) : (3.0f - 4.0f * t);
}

// Generate waveform sample based on type
static float generate_waveform(pfxr_wave_type_t wave_type, float phase) {
    switch (wave_type) {
        case PFXR_WAVE_SINE:
            return generate_sine(phase);
        case PFXR_WAVE_SAWTOOTH:
            return generate_sawtooth(phase);
        case PFXR_WAVE_SQUARE:
            return generate_square(phase);
        case PFXR_WAVE_TRIANGLE:
            return generate_triangle(phase);
        default:
            return generate_sine(phase);
    }
}

// Simple biquad filter implementation
typedef struct {
    float a0, a1, a2, b1, b2;
    float x1, x2, y1, y2;
} biquad_filter_t;

static void biquad_lowpass_coeffs(biquad_filter_t* filter, float freq, float q, float sample_rate) {
    float w = 2.0f * M_PI * freq / sample_rate;
    float cos_w = cosf(w);
    float sin_w = sinf(w);
    float alpha = sin_w / (2.0f * q);
    
    float b0 = (1.0f - cos_w) / 2.0f;
    float b1 = 1.0f - cos_w;
    float b2 = (1.0f - cos_w) / 2.0f;
    float a0 = 1.0f + alpha;
    float a1 = -2.0f * cos_w;
    float a2 = 1.0f - alpha;
    
    filter->a0 = b0 / a0;
    filter->a1 = b1 / a0;
    filter->a2 = b2 / a0;
    filter->b1 = a1 / a0;
    filter->b2 = a2 / a0;
}

static void biquad_highpass_coeffs(biquad_filter_t* filter, float freq, float q, float sample_rate) {
    float w = 2.0f * M_PI * freq / sample_rate;
    float cos_w = cosf(w);
    float sin_w = sinf(w);
    float alpha = sin_w / (2.0f * q);
    
    float b0 = (1.0f + cos_w) / 2.0f;
    float b1 = -(1.0f + cos_w);
    float b2 = (1.0f + cos_w) / 2.0f;
    float a0 = 1.0f + alpha;
    float a1 = -2.0f * cos_w;
    float a2 = 1.0f - alpha;
    
    filter->a0 = b0 / a0;
    filter->a1 = b1 / a0;
    filter->a2 = b2 / a0;
    filter->b1 = a1 / a0;
    filter->b2 = a2 / a0;
}

static float biquad_process(biquad_filter_t* filter, float input) {
    float output = filter->a0 * input + filter->a1 * filter->x1 + filter->a2 * filter->x2
                  - filter->b1 * filter->y1 - filter->b2 * filter->y2;
    
    filter->x2 = filter->x1;
    filter->x1 = input;
    filter->y2 = filter->y1;
    filter->y1 = output;
    
    return output;
}

// Generate noise curve for distortion effect
static float generate_noise_distortion(float input, float noise_amount, uint32_t* noise_seed) {
    if (noise_amount <= 0.0f) return input;
    
    // Simple linear congruential generator for deterministic noise
    *noise_seed = (*noise_seed * 1103515245 + 12345) & 0x7fffffff;
    float rand1 = (float)(*noise_seed) / (float)0x7fffffff;
    
    *noise_seed = (*noise_seed * 1103515245 + 12345) & 0x7fffffff;
    float rand2 = (float)(*noise_seed) / (float)0x7fffffff;
    
    float deg = M_PI / 180.0f;
    float noise_factor = (3.0f + rand1 * noise_amount);
    float distortion = (noise_factor * input * 20.0f * deg) / 
                      (M_PI + rand2 * noise_amount * fabsf(input));
    
    return clamp(distortion, -1.0f, 1.0f);
}

// Main sound generation function  
void pfxr_generate_sound(const pfxr_sound_t* config, pfxr_audio_buffer_t* buffer) {
    if (!config || !buffer) return;
    
    float sample_rate = (float)PFXR_SAMPLE_RATE;
    float duration = config->attackTime + config->sustainTime + config->decayTime;
    int total_samples = (int)(duration * sample_rate);
    
    if (total_samples > buffer->capacity) {
        total_samples = buffer->capacity;
    }
    
    // Initialize noise seed based on config parameters for deterministic noise
    uint32_t noise_seed = (uint32_t)(config->frequency * 1000 + config->noiseAmount * 100 + config->volume * 1000);
    
    // Initialize filters
    biquad_filter_t lowpass_filter = {0};
    biquad_filter_t highpass_filter = {0};
    
    if (config->lowPassCutoff > 0.0f) {
        float q = config->lowPassResonance > 0.0f ? config->lowPassResonance : 0.707f;
        biquad_lowpass_coeffs(&lowpass_filter, config->lowPassCutoff, q, sample_rate);
    }
    
    if (config->highPassCutoff > 0.0f) {
        float q = config->highPassResonance > 0.0f ? config->highPassResonance : 0.707f;
        biquad_highpass_coeffs(&highpass_filter, config->highPassCutoff, q, sample_rate);
    }
    
    // Generate samples
    float phase = 0.0f;
    float vibrato_phase = 0.0f;
    float tremolo_phase = 0.0f;
    float phaser_phase = 0.0f;
    
    for (int i = 0; i < total_samples; i++) {
        float t = (float)i / sample_rate;
        float sample = 0.0f;
        
        // Calculate envelope
        float envelope = 0.0f;
        if (t < config->attackTime) {
            // Attack phase
            envelope = (1.0f - config->sustainPunch) * (t / config->attackTime);
        } else if (t < config->attackTime + config->sustainTime) {
            // Sustain phase
            envelope = 1.0f;
        } else {
            // Decay phase
            float decay_t = (t - config->attackTime - config->sustainTime) / config->decayTime;
            envelope = (1.0f - config->sustainPunch) * (1.0f - decay_t);
        }
        
        if (envelope < 0.0f) envelope = 0.0f;
        
        // Calculate frequency with pitch sweep
        float current_freq = config->frequency;
        if (config->pitchDelta != 0.0f && t >= config->pitchDelay) {
            float pitch_t = (t - config->pitchDelay) / (duration - config->pitchDelay);
            if (pitch_t > config->pitchDuration) pitch_t = config->pitchDuration;
            current_freq += config->pitchDelta * pitch_t;
        }
        
        // Apply vibrato
        if (config->vibratoRate > 0.0f && config->vibratoDepth > 0.0f) {
            float vibrato = sinf(vibrato_phase) * config->vibratoDepth;
            current_freq += vibrato;
            vibrato_phase += (config->vibratoRate * 2.0f * M_PI) / sample_rate;
        }
        
        // Generate base waveform
        if (current_freq > 0.0f) {
            sample = generate_waveform((pfxr_wave_type_t)config->waveForm, phase);
            phase += current_freq / sample_rate;
            if (phase >= 1.0f) phase -= 1.0f;
        }
        
        // Apply noise distortion
        if (config->noiseAmount > 0.0f) {
            sample = generate_noise_distortion(sample, config->noiseAmount / 100.0f, &noise_seed);
        }
        
        // Apply phaser effect (simplified)
        if (config->phaserDepth > 0.0f) {
            float phaser_freq = config->phaserBaseFrequency + 
                               sinf(phaser_phase) * config->phaserDepth;
            // Simplified phaser - just add a delayed version
            float delay_samples = sample_rate / (phaser_freq + 1.0f);
            if (i >= (int)delay_samples) {
                sample += buffer->samples[i - (int)delay_samples] * 0.5f;
            }
            phaser_phase += (config->phaserLfoFrequency * 2.0f * M_PI) / sample_rate;
        }
        
        // Apply filters
        if (config->lowPassCutoff > 0.0f && config->lowPassCutoff < 4000.0f) {
            sample = biquad_process(&lowpass_filter, sample);
        }
        
        if (config->highPassCutoff > 0.0f) {
            sample = biquad_process(&highpass_filter, sample);
        }
        
        // Apply envelope
        sample *= envelope;
        
        // Apply tremolo
        if (config->tremoloRate > 0.0f && config->tremoloDepth > 0.0f) {
            float tremolo = 1.0f - config->tremoloDepth * (1.0f + sinf(tremolo_phase)) * 0.5f;
            sample *= tremolo;
            tremolo_phase += (config->tremoloRate * 2.0f * M_PI) / sample_rate;
        }
        
        // Apply volume and clamp
        sample *= config->volume;
        sample = clamp(sample, -1.0f, 1.0f);
        
        buffer->samples[i] = sample;
    }
    
    buffer->sample_count = total_samples;
}

// ============================================================================
// WAV FILE IMPLEMENTATION
// ============================================================================

// Create WAV data from float samples
char* pfxr_create_wav_data(const float* samples, int sample_count, int* wav_size) {
    if (!samples || sample_count <= 0 || !wav_size) {
        return NULL;
    }
    
    // Calculate sizes
    int data_size = sample_count * sizeof(int16_t);
    int file_size = sizeof(pfxr_wav_header_t) + data_size;
    
    // Allocate memory for WAV data
    char* wav_data = malloc(file_size);
    if (!wav_data) {
        return NULL;
    }
    
    // Create WAV header
    pfxr_wav_header_t* header = (pfxr_wav_header_t*)wav_data;
    
    // RIFF header
    memcpy(header->riff, "RIFF", 4);
    header->chunk_size = file_size - 8;
    memcpy(header->wave, "WAVE", 4);
    
    // Format chunk
    memcpy(header->fmt, "fmt ", 4);
    header->fmt_size = 16;
    header->audio_format = 1;  // PCM
    header->num_channels = 1;  // Mono
    header->sample_rate = PFXR_SAMPLE_RATE;
    header->bits_per_sample = 16;
    header->block_align = header->num_channels * header->bits_per_sample / 8;
    header->byte_rate = header->sample_rate * header->block_align;
    
    // Data chunk
    memcpy(header->data, "data", 4);
    header->data_size = data_size;
    
    // Convert float samples to 16-bit PCM
    int16_t* pcm_data = (int16_t*)(wav_data + sizeof(pfxr_wav_header_t));
    for (int i = 0; i < sample_count; i++) {
        // Clamp and scale to 16-bit range
        float sample = samples[i];
        if (sample > 1.0f) sample = 1.0f;
        if (sample < -1.0f) sample = -1.0f;
        
        pcm_data[i] = (int16_t)(sample * 32767.0f);
    }
    
    *wav_size = file_size;
    return wav_data;
}

// Write WAV file to disk
int pfxr_write_wav_file(const char* filename, const float* samples, int sample_count) {
    if (!filename || !samples || sample_count <= 0) {
        return -1;
    }
    
    int wav_size;
    char* wav_data = pfxr_create_wav_data(samples, sample_count, &wav_size);
    if (!wav_data) {
        return -1;
    }
    
    FILE* file = fopen(filename, "wb");
    if (!file) {
        free(wav_data);
        return -1;
    }
    
    size_t written = fwrite(wav_data, 1, wav_size, file);
    fclose(file);
    free(wav_data);
    
    return (written == (size_t)wav_size) ? 0 : -1;
}

// Free WAV data allocated by pfxr_create_wav_data
void pfxr_free_wav_data(char* wav_data) {
    if (wav_data) {
        free(wav_data);
    }
}

// ============================================================================
// URL IMPLEMENTATION
// ============================================================================

// Helper function to URL decode a string
static char* url_decode(const char* encoded) {
    if (!encoded) return NULL;

    size_t len = strlen(encoded);
    char* decoded = malloc(len + 1);
    if (!decoded) return NULL;

    size_t i = 0, j = 0;
    while (i < len) {
        if (encoded[i] == '%' && i + 2 < len) {
            // Convert hex to char
            char hex[3] = {encoded[i+1], encoded[i+2], '\0'};
            char* endptr;
            long val = strtol(hex, &endptr, 16);
            if (*endptr == '\0') {
                decoded[j++] = (char)val;
                i += 3;
            } else {
                decoded[j++] = encoded[i++];
            }
        } else if (encoded[i] == '+') {
            decoded[j++] = ' ';
            i++;
        } else {
            decoded[j++] = encoded[i++];
        }
    }
    decoded[j] = '\0';
    return decoded;
}

// Helper function to URL encode a string
static char* url_encode(const char* str) {
    if (!str) return NULL;

    size_t len = strlen(str);
    // Worst case: every character needs encoding (3 chars each)
    char* encoded = malloc(len * 3 + 1);
    if (!encoded) return NULL;

    size_t j = 0;
    for (size_t i = 0; i < len; i++) {
        char c = str[i];
        if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
            encoded[j++] = c;
        } else if (c == ' ') {
            encoded[j++] = '+';
        } else {
            sprintf(&encoded[j], "%%%02X", (unsigned char)c);
            j += 3;
        }
    }
    encoded[j] = '\0';
    return encoded;
}

// Helper function to find query parameter value
static char* get_query_param(const char* url, const char* param) {
    if (!url || !param) return NULL;

    // Find the start of query string
    const char* query_start = strchr(url, '?');
    if (!query_start) return NULL;
    query_start++; // Skip the '?'

    // Look for the parameter
    char param_with_eq[256];
    snprintf(param_with_eq, sizeof(param_with_eq), "%s=", param);

    const char* param_pos = strstr(query_start, param_with_eq);
    if (!param_pos) return NULL;

    // Move to start of value
    const char* value_start = param_pos + strlen(param_with_eq);

    // Find end of value (next & or end of string)
    const char* value_end = value_start;
    while (*value_end && *value_end != '&' && *value_end != '#') {
        value_end++;
    }

    // Copy the value
    size_t value_len = value_end - value_start;
    char* value = malloc(value_len + 1);
    if (!value) return NULL;

    strncpy(value, value_start, value_len);
    value[value_len] = '\0';

    return value;
}

// Helper function to set sound field by index
static void set_sound_field(pfxr_sound_t* sound, int index, float value) {
    switch (index) {
        case 0: sound->waveForm = (int)value; break;
        case 1: sound->volume = value; break;
        case 2: sound->attackTime = value; break;
        case 3: sound->sustainTime = value; break;
        case 4: sound->sustainPunch = value; break;
        case 5: sound->decayTime = value; break;
        case 6: sound->frequency = value; break;
        case 7: sound->pitchDelta = value; break;
        case 8: sound->pitchDuration = value; break;
        case 9: sound->pitchDelay = value; break;
        case 10: sound->vibratoRate = value; break;
        case 11: sound->vibratoDepth = value; break;
        case 12: sound->tremoloRate = value; break;
        case 13: sound->tremoloDepth = value; break;
        case 14: sound->highPassCutoff = value; break;
        case 15: sound->highPassResonance = value; break;
        case 16: sound->lowPassCutoff = value; break;
        case 17: sound->lowPassResonance = value; break;
        case 18: sound->phaserBaseFrequency = value; break;
        case 19: sound->phaserLfoFrequency = value; break;
        case 20: sound->phaserDepth = value; break;
        case 21: sound->noiseAmount = value; break;
        default: break; // Ignore unknown indices
    }
}

// Helper function to get sound field by index
static float get_sound_field(const pfxr_sound_t* sound, int index) {
    switch (index) {
        case 0: return (float)sound->waveForm;
        case 1: return sound->volume;
        case 2: return sound->attackTime;
        case 3: return sound->sustainTime;
        case 4: return sound->sustainPunch;
        case 5: return sound->decayTime;
        case 6: return sound->frequency;
        case 7: return sound->pitchDelta;
        case 8: return sound->pitchDuration;
        case 9: return sound->pitchDelay;
        case 10: return sound->vibratoRate;
        case 11: return sound->vibratoDepth;
        case 12: return sound->tremoloRate;
        case 13: return sound->tremoloDepth;
        case 14: return sound->highPassCutoff;
        case 15: return sound->highPassResonance;
        case 16: return sound->lowPassCutoff;
        case 17: return sound->lowPassResonance;
        case 18: return sound->phaserBaseFrequency;
        case 19: return sound->phaserLfoFrequency;
        case 20: return sound->phaserDepth;
        case 21: return sound->noiseAmount;
        default: return 0.0f;
    }
}

// Number of fields in the sound structure
#define PFXR_FIELD_COUNT 22

pfxr_sound_t* pfxr_create_params_from_url(const char* url) {
    if (!url) return NULL;

    // Allocate memory for the sound configuration
    pfxr_sound_t* sound = malloc(sizeof(pfxr_sound_t));
    if (!sound) return NULL;

    // Initialize with default values
    *sound = pfxr_get_default_sound();

    // Get the 'fx' query parameter
    char* fx_param = get_query_param(url, "fx");
    if (!fx_param) {
        return sound; // Return default sound if no fx parameter
    }

    // URL decode the parameter
    char* decoded = url_decode(fx_param);
    free(fx_param);
    if (!decoded) {
        return sound; // Return default sound if decoding fails
    }

    // Split by comma and parse values
    char* token = strtok(decoded, ",");
    int index = 0;

    while (token && index < PFXR_FIELD_COUNT) {
        char* endptr;
        float value = strtof(token, &endptr);

        // Only set the value if it's a valid number
        if (*endptr == '\0' || *endptr == ' ') {
            set_sound_field(sound, index, value);
        }

        token = strtok(NULL, ",");
        index++;
    }

    free(decoded);
    return sound;
}

char* pfxr_get_url_from_params(const pfxr_sound_t* config) {
    if (!config) return NULL;

    // Calculate buffer size needed
    // Each float can be up to ~15 characters, plus comma
    size_t buffer_size = PFXR_FIELD_COUNT * 16 + 100; // Extra space for URL parts
    char* url_buffer = malloc(buffer_size);
    if (!url_buffer) return NULL;

    // Create comma-separated values string
    char values_buffer[PFXR_FIELD_COUNT * 16];
    values_buffer[0] = '\0';

    for (int i = 0; i < PFXR_FIELD_COUNT; i++) {
        float value = get_sound_field(config, i);
        char value_str[16];

        // Format the value, removing unnecessary decimal places
        if (i == 0) { // waveForm is an integer
            snprintf(value_str, sizeof(value_str), "%d", (int)value);
        } else {
            snprintf(value_str, sizeof(value_str), "%.6g", value);
        }

        if (i > 0) {
            strcat(values_buffer, ",");
        }
        strcat(values_buffer, value_str);
    }

    // URL encode the values
    char* encoded_values = url_encode(values_buffer);
    if (!encoded_values) {
        free(url_buffer);
        return NULL;
    }

    // Create the URL string with the fx parameter
    snprintf(url_buffer, buffer_size, "?fx=%s", encoded_values);

    free(encoded_values);
    return url_buffer;
}

void pfxr_free_sound_config(pfxr_sound_t* config) {
    if (config) {
        free(config);
    }
}

// ============================================================================
// MAIN API FUNCTIONS IMPLEMENTATION
// ============================================================================

// Main API function to create sound from template and return WAV data
char* pfxr_create_sound_from_template(pfxr_template_t template, int seed) {
    // Apply template to get sound configuration
    pfxr_sound_t config = pfxr_apply_template(template, seed);
    
    // Generate sound from configuration
    return pfxr_create_sound_from_config(&config);
}

// Create sound from template and save to file
int pfxr_create_sound_from_template_to_file(pfxr_template_t template, int seed, const char* filename) {
    if (!filename) {
        return -1;
    }
    
    // Apply template to get sound configuration
    pfxr_sound_t config = pfxr_apply_template(template, seed);
    
    // Generate sound from configuration and save to file
    return pfxr_create_sound_from_config_to_file(&config, filename);
}

// Create sound from configuration and return WAV data
char* pfxr_create_sound_from_config(const pfxr_sound_t* config) {
    if (!config) {
        return NULL;
    }
    
    // Create audio buffer
    pfxr_audio_buffer_t* buffer = pfxr_create_audio_buffer(PFXR_MAX_SAMPLES);
    if (!buffer) {
        return NULL;
    }
    
    // Generate sound
    pfxr_generate_sound(config, buffer);
    
    // Convert to WAV data
    int wav_size;
    char* wav_data = pfxr_create_wav_data(buffer->samples, buffer->sample_count, &wav_size);
    
    // Clean up
    pfxr_free_audio_buffer(buffer);
    
    return wav_data;
}

// Create sound from configuration and save to file
int pfxr_create_sound_from_config_to_file(const pfxr_sound_t* config, const char* filename) {
    if (!config || !filename) {
        return -1;
    }
    
    // Create audio buffer
    pfxr_audio_buffer_t* buffer = pfxr_create_audio_buffer(PFXR_MAX_SAMPLES);
    if (!buffer) {
        return -1;
    }
    
    // Generate sound
    pfxr_generate_sound(config, buffer);
    
    // Write to file
    int result = pfxr_write_wav_file(filename, buffer->samples, buffer->sample_count);
    
    // Clean up
    pfxr_free_audio_buffer(buffer);
    
    return result;
}

#endif // PFXR_IMPLEMENTATION

#endif // PFXR_H