#include "../include/pfxr.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

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